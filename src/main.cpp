#include "../include/main.hpp"
#include "../include/codegen.hpp"
#include "../include/UI.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "../extern/questui/shared/QuestUI.hpp"
#include "../extern/questui/shared/BeatSaberUI.hpp"
#include "../extern/custom-types/shared/register.hpp"

#include <sstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <limits>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <string.h>

using namespace UnityEngine::UI;
using namespace il2cpp_utils;
using namespace GlobalNamespace;

static float clamp(float num, float min, float max) {
    if(num < min) {
        return min;
    } else if(num > max) {
        return max;
    } else {
        return num;
    }
}

Logger& logger() {
    static auto logger = new Logger(modInfo, LoggerOptions(false, false));
    return *logger;
}

#define log(...) logger().info(__VA_ARGS__)

Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

int triggerNode;
float rTriggerVal;
float lTriggerVal;

bool skipReady;

float songTime;
float skipTime;
float timeToNextNote;

float firstNoteTime;
float lastNoteTime = 0;

float timeBeforeNoteToSkipTo;

float timeToHold;
float timeHeld = 0;

float minSkipTime;

int objectCount = 0;

float songLength;

TMPro::TextMeshProUGUI* scoreText;

bool skipTextIsDestroyed = true;
bool inSong = false;

float textX;
float textY;
int textSize;
UnityEngine::Color rgbColors;

bool inPauseMenu;

bool gotTimes;

static void SaveConfig() {
    if (!getConfig().config.HasMember("Are Outro Skips Enabled")) {
        // log("Regenerating config!");
        getConfig().config.SetObject();
        auto& allocator = getConfig().config.GetAllocator();
        getConfig().config.AddMember("Text X Offset", 0.0, allocator);
        getConfig().config.AddMember("Text Y Offset", 0.0, allocator);
        getConfig().config.AddMember("Text Size", 34, allocator);
        getConfig().config.AddMember("Red Color", 255.0, allocator);
        getConfig().config.AddMember("Green Color", 255.0, allocator);
        getConfig().config.AddMember("Blue Color", 255.0, allocator);
        getConfig().config.AddMember("Time Held To Skip", 0.2, allocator);
        getConfig().config.AddMember("How Long Before The Notes To Skip To", 2.0, allocator);
        getConfig().config.AddMember("Minimum Time Needed To Be Able To Skip", 7, allocator);
        getConfig().config.AddMember("Are Middle Skips Enabled", true, allocator);
        getConfig().config.AddMember("Are Intro Skips Enabled", true, allocator);
        getConfig().config.AddMember("Are Outro Skips Enabled", true, allocator);
        getConfig().Write();
        // log("Config regeneration complete!");
    }
    else {
        // log("Not regnerating config.");
    }
}

template<class T>
UnityEngine::GameObject* FindObject(std::string name, bool byParent = true, bool getLastIndex = false) {
    // log("Finding GameObject of name "+name);
    Array<T>* trs = UnityEngine::Resources::FindObjectsOfTypeAll<T>();
    // log("There are "+std::to_string(trs->Length())+" GameObjects");
    for(int i = 0; i < trs->Length(); i++) {
        if(i != trs->Length()-1 && getLastIndex) continue;
        UnityEngine::GameObject* go = trs->values[i]->get_gameObject();
        if(byParent) {
            go = go->get_transform()->GetParent()->get_gameObject();
        }
        // log(to_utf8(csstrtostr(UnityEngine::Transform::GetName(trs->values[i]))));
        if(to_utf8(csstrtostr(UnityEngine::Transform::GetName(go))) == name){
            // log("Found GameObject");
            return go;
        }
    }
    // log("Could not find GameObject");
    return nullptr;
}

void onNoteSpawn(Il2CppObject* noteData) {
    objectCount++;

    timeToNextNote = *il2cpp_utils::RunMethod<float>(noteData, "get_timeToNextColorNote");

    // log("Time to next note is "+std::to_string(timeToNextNote));

    if(timeToNextNote > minSkipTime && timeToNextNote < songLength && getConfig().config["Are Middle Skips Enabled"].GetBool() && !skipReady) {
        skipReady = true;
        skipTime = songTime + timeToNextNote - timeBeforeNoteToSkipTo;
        // log("Skip is ready, skip time is %f", skipTime);
    }
}

MAKE_HOOK_FIND_CLASS(SongUpdate, "", "AudioTimeSyncController", "Update", void, Il2CppObject* self) {
    
    SongUpdate(self);

    if(!gotTimes) {
        float startTime = *GetFieldValue<float>(self, "_startSongTime");
        // log("Start time is %f", startTime);
        auto bocc = UnityEngine::Object::FindObjectOfType<GlobalNamespace::BeatmapObjectCallbackController*>();
        auto beatmapLinesData = reinterpret_cast<GlobalNamespace::BeatmapData*>(bocc->initData->beatmapData)->beatmapLinesData;
        for (int i = 0; i < beatmapLinesData->Length(); i++) {
            auto beatmapLineData = beatmapLinesData->values[i];
            auto beatmapObjectsData = beatmapLineData->beatmapObjectsData->items;
            for (int j = 0; j < beatmapObjectsData->Length(); j++) {
                auto beatmapObjectData = beatmapObjectsData->values[j];
                if (!beatmapObjectData) continue;
                if (*RunMethod<int>(beatmapObjectData, "get_beatmapObjectType") == 0) {
                    auto note = reinterpret_cast<GlobalNamespace::NoteData*>(beatmapObjectData);
                    if (note->colorType != GlobalNamespace::ColorType::None) {
                        float noteTime = note->get_time();
                        if(noteTime < firstNoteTime && noteTime > startTime) {
                            firstNoteTime = noteTime;
                        }
                        if(noteTime > lastNoteTime) {
                            lastNoteTime = noteTime;
                        }
                    }
                }
            }
        }
        // log("First note time is %f, last note time is %f", firstNoteTime, lastNoteTime);
        gotTimes = true;
    }

    songTime = *il2cpp_utils::GetFieldValue<float>(self, "_songTime");
    Il2CppObject* audioSource = *il2cpp_utils::GetFieldValue(self, "_audioSource");

    if(!skipReady) {
        if(songTime < firstNoteTime - timeBeforeNoteToSkipTo - 0.5f && firstNoteTime > minSkipTime && getConfig().config["Are Intro Skips Enabled"].GetBool()) {
            skipReady = true;
            skipTime = firstNoteTime - timeBeforeNoteToSkipTo;
            // log("Skip is ready, skip time is %f", skipTime);
        }
        if(songTime > lastNoteTime && songTime < songLength-2 && getConfig().config["Are Outro Skips Enabled"].GetBool()) {
            skipReady = true;
            skipTime = songLength-2;
            objectCount = 0;
            // log("Skip is ready, skip time is %f", skipTime);
        }
    }
    if(songTime > skipTime) {
        skipReady = false;
    }
    if(rTriggerVal > 0.8 && lTriggerVal > 0.8 && objectCount == 0 && !inPauseMenu) {
        timeHeld += UnityEngine::Time::get_deltaTime();
    } else {
        timeHeld = 0;
    }

    if(skipReady && timeHeld+0.05f >= timeToHold && songTime < skipTime - 0.5f && songTime > 0.1f && objectCount == 0) {
        // log("Skipped to %f", skipTime);
        il2cpp_utils::RunMethod(audioSource, "set_time", skipTime);

        skipReady = false;
        timeHeld = 0;
    }
    
    if(skipReady && inSong && objectCount == 0) {
        std::string tempString = std::to_string(timeToHold - timeHeld);
        tempString.erase(3, tempString.length()-1);

        std::string text = "Hold triggers to skip";

        if(scoreText == nullptr && songTime > 0.1f) {
            bool getLastIndex = false;
            // if(FindObject<MultiplayerController*>("MultiplayerController", false) == nullptr) {
            //     getLastIndex = true;
            // }
            // log("Get last index is %i", getLastIndex);
            scoreText = QuestUI::BeatSaberUI::CreateText(FindObject<ComboUIController*>("ComboPanel", false, getLastIndex)->get_transform(), text, UnityEngine::Vector2{textX*10, 57+(textY*10)});
            scoreText->set_color(UnityEngine::Color{rgbColors.r, rgbColors.g, rgbColors.b, 1});
            scoreText->set_alignment(TMPro::TextAlignmentOptions::Center);
            scoreText->set_fontSize(textSize);
            scoreText->set_lineSpacing(-30);
        } else {
            if(scoreText != nullptr) {
                scoreText->get_gameObject()->SetActive(true);
                scoreText->SetText(createcsstr(text));
            }
        }
    } else {
        if(scoreText != nullptr) {
            scoreText->get_gameObject()->SetActive(false);
        }
    }
}

MAKE_HOOK_FIND_CLASS(SongEnd, "", "StandardLevelScenesTransitionSetupDataSO", "Finish", void, Il2CppObject* self, Il2CppObject* levelCompleteionResults) {
    
    inSong = false;

    objectCount = 0;
    
    SongEnd(self, levelCompleteionResults);
}

MAKE_HOOK_FIND_CLASS(SongStart, "", "AudioTimeSyncController", "Awake", void, Il2CppObject* self) {

    // log("SongStart");

    firstNoteTime = songLength;
    lastNoteTime = -1;
    inSong = true;
    gotTimes = false;
    scoreText = nullptr;
    inPauseMenu = false;
    skipReady = false;
    skipTime = 0;
    
    timeBeforeNoteToSkipTo = std::max(0.0f, getConfig().config["How Long Before The Notes To Skip To"].GetFloat());
    timeToHold = std::max(0.0f, getConfig().config["Time Held To Skip"].GetFloat());
    minSkipTime = std::max(2.5f, getConfig().config["Minimum Time Needed To Be Able To Skip"].GetFloat());
    textX = getConfig().config["Text X Offset"].GetFloat();
    textY = getConfig().config["Text Y Offset"].GetFloat();
    textSize = getConfig().config["Text Size"].GetInt();
    rgbColors = {clamp(getConfig().config["Red Color"].GetFloat(), 0.0f, 255.0f), clamp(getConfig().config["Green Color"].GetFloat(), 0.0f, 255.0f), clamp(getConfig().config["Blue Color"].GetFloat(), 0.0f, 255.0f)};

    SongStart(self);
}

MAKE_HOOK_FIND_CLASS(Triggers, "", "VRControllersInputManager", "TriggerValue", void, Il2CppObject* self, int node) {

    triggerNode = node;

    Triggers(self, node);
}

MAKE_HOOK_FIND_CLASS(ControllerUpdate, "", "VRController", "Update", void, Il2CppObject* self) {

    float trigger = *il2cpp_utils::RunMethod<float>(self, "get_triggerValue");

    if (triggerNode == 4) {
        lTriggerVal = trigger;
    }
    if (triggerNode == 5) {
        rTriggerVal = trigger;
    }

    ControllerUpdate(self);
}

MAKE_HOOK_FIND_CLASS(SpawnNote, "", "BeatmapObjectSpawnController", "SpawnBasicNote", void, Il2CppObject* self, Il2CppObject* noteData, float cutDirectionAngleOffset) {

    onNoteSpawn(noteData);

    SpawnNote(self, noteData, cutDirectionAngleOffset);
}

MAKE_HOOK_FIND_CLASS(SpawnBomb, "", "BeatmapObjectSpawnController", "SpawnBombNote", void, Il2CppObject* self, Il2CppObject* noteData) {

    onNoteSpawn(noteData);

    SpawnBomb(self, noteData);
}

MAKE_HOOK_FIND_CLASS(NoteCut, "", "BeatmapObjectManager", "HandleNoteWasMissed", void, Il2CppObject* self, Il2CppObject* noteController, Il2CppObject* noteCutInfo) {

    objectCount--;

    NoteCut(self, noteController, noteCutInfo);
}

MAKE_HOOK_FIND_CLASS(NoteMissed, "", "BeatmapObjectManager", "HandleNoteWasCut", void, Il2CppObject* self, Il2CppObject* noteController) {

    objectCount--;

    NoteMissed(self, noteController);
}

MAKE_HOOK_FIND_CLASS(GetLength, "", "StandardLevelDetailView", "RefreshContent", void, Il2CppObject* self) {

    GetLength(self);

    Il2CppObject* level = *il2cpp_utils::GetFieldValue(self, "_level");
    Il2CppObject* beatmapLevelData = *il2cpp_utils::RunMethod(level, "get_beatmapLevelData");
    Il2CppObject* audioClip = *il2cpp_utils::RunMethod(beatmapLevelData, "get_audioClip");
    songLength = *il2cpp_utils::RunMethod<float>(audioClip, "get_length");
}

MAKE_HOOK_FIND_CLASS(PauseStart, "", "PauseMenuManager", "ShowMenu", void, Il2CppObject* self) {

    // log("PauseStart");

    PauseStart(self);

    inPauseMenu = true;
}

MAKE_HOOK_FIND_CLASS(PauseAnimationFinish, "", "PauseController", "HandlePauseMenuManagerDidFinishResumeAnimation", void, Il2CppObject* self) {

    // log("PauseAnimationFinish");

    PauseAnimationFinish(self);

    inPauseMenu = false;
}

extern "C" void setup(ModInfo& info) {
    info.id = "IntroSkip";
    info.version = "0.2.0";
    modInfo = info;
    // Create logger
    // static std::unique_ptr<const Logger> ptr(new Logger(info));
    // logger().get().info("Completed setup!");
    // We can even check information specific to the modloader!
    // logger().get().info("Modloader name: %s", Modloader::getInfo().name.c_str());
    
    getConfig().Load();
    SaveConfig();
    timeBeforeNoteToSkipTo = std::max(0.0f, getConfig().config["How Long Before The Notes To Skip To"].GetFloat());
    timeToHold = std::max(0.0f, getConfig().config["Time Held To Skip"].GetFloat());
    minSkipTime = std::max(0.0f, getConfig().config["Minimum Time Needed To Be Able To Skip"].GetFloat());
    textX = getConfig().config["Text X Offset"].GetFloat();
    textY = getConfig().config["Text Y Offset"].GetFloat();
    textSize = getConfig().config["Text Size"].GetFloat();
    rgbColors = {clamp(getConfig().config["Red Color"].GetFloat()/255, 0.0f, 1.0f), clamp(getConfig().config["Green Color"].GetFloat()/255, 0.0f, 1.0f), clamp(getConfig().config["Blue Color"].GetFloat()/255, 0.0f, 1.0f)};
}

extern "C" void load() {
    QuestUI::Init();

    custom_types::Register::AutoRegister();
    QuestUI::Register::RegisterModSettingsViewController<IntroSkip::UIController*>(modInfo, "IntroSkip");
}