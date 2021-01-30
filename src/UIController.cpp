#include "../include/UI.hpp"
#include "../include/codegen.hpp"
using namespace QuestUI;
using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_CLASS(IntroSkip::UIController);

void toggleMiddleSkips() {
    getConfig().config["Are Middle Skips Enabled"].SetBool(!getConfig().config["Are Middle Skips Enabled"].GetBool());
}

void toggleIntroSkips() {
    getConfig().config["Are Intro Skips Enabled"].SetBool(!getConfig().config["Are Intro Skips Enabled"].GetBool());
}

void toggleOutroSkips() {
    getConfig().config["Are Outro Skips Enabled"].SetBool(!getConfig().config["Are Outro Skips Enabled"].GetBool());
}

void changeTextSize(float newValue) {
    getConfig().config["Text Size"].SetInt(newValue);
}

void changeTextXOffset(float newValue) {
    getConfig().config["Text X Offset"].SetInt(newValue);
}

void changeTextYOffset(float newValue) {
    getConfig().config["Text Y Offset"].SetInt(newValue);
}

void changeHoldTime(float newValue) {
    getConfig().config["Time Held To Skip"].SetFloat(newValue);
}

void changeMinimumSkipTime(float newValue) {
    getConfig().config["Minimum Time Needed To Be Able To Skip"].SetFloat(newValue);
}

void changeTimeBefore(float newValue) {
    getConfig().config["How Long Before The Notes To Skip To"].SetFloat(newValue);
}

void IntroSkip::UIController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
	if(firstActivation)
	{
		VerticalLayoutGroup* layout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
		layout->set_spacing(1.5f);
        
        float spacing = 0.7f;
		
		VerticalLayoutGroup* layout1 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(layout->get_rectTransform());
		layout1->set_spacing(spacing);
		layout1->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("round-rect-panel"));
		
		VerticalLayoutGroup* layout2 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(layout->get_rectTransform());
		layout2->set_spacing(spacing);
		layout2->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("round-rect-panel"));
		
		VerticalLayoutGroup* layout3 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(layout->get_rectTransform());
		layout3->set_spacing(spacing);
		layout3->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("round-rect-panel"));

		layout1->set_padding(UnityEngine::RectOffset::New_ctor(3, 3, 2, 2));
		layout2->set_padding(UnityEngine::RectOffset::New_ctor(3, 3, 2, 2));
		layout3->set_padding(UnityEngine::RectOffset::New_ctor(3, 3, 2, 2));

		Transform* Parent1 = layout1->get_transform();
		Transform* Parent2 = layout2->get_transform();
		Transform* Parent3 = layout3->get_transform();

		Toggle* IntroSkipsToggle = BeatSaberUI::CreateToggle(
            Parent1,
            "Enable Intro Skips",
            getConfig().config["Are Intro Skips Enabled"].GetBool(),
            il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(classof(UnityEngine::Events::UnityAction_1<bool>*), this, toggleIntroSkips)
        );
		QuestUI::BeatSaberUI::AddHoverHint(IntroSkipsToggle->get_gameObject(), "Songs without notes during the intro will be skippable when enabled");

		Toggle* SkipsToggle = BeatSaberUI::CreateToggle(
            Parent1,
            "Enable Middle Skips",
            getConfig().config["Are Middle Skips Enabled"].GetBool(),
            il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(classof(UnityEngine::Events::UnityAction_1<bool>*), this, toggleMiddleSkips)
        );
		QuestUI::BeatSaberUI::AddHoverHint(SkipsToggle->get_gameObject(), "Pauses in the middle of songs will be skippable when enabled");

		Toggle* OutroSkipsToggle = BeatSaberUI::CreateToggle(
            Parent1,
            "Enable Outro Skips",
            getConfig().config["Are Outro Skips Enabled"].GetBool(),
            il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(classof(UnityEngine::Events::UnityAction_1<bool>*), this, toggleOutroSkips)
        );
		QuestUI::BeatSaberUI::AddHoverHint(OutroSkipsToggle->get_gameObject(), "Songs without notes during the outro will be skippable when enabled");

        QuestUI::IncrementSetting* HoldTime = BeatSaberUI::CreateIncrementSetting(
            Parent2,
            "Hold Time",
            1,
            0.1f,
            getConfig().config["Time Held To Skip"].GetFloat(),
            0,
            5,
            il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(classof(UnityEngine::Events::UnityAction_1<float>*), this, changeHoldTime)
        );
		QuestUI::BeatSaberUI::AddHoverHint(HoldTime->get_gameObject(), "Changes the amount of time you have to hold the triggers to skip a section, setting to 0 will cause automatic skipping");

        QuestUI::IncrementSetting* MinimumSkipTime = BeatSaberUI::CreateIncrementSetting(
            Parent2,
            "Minimum Skip Time",
            1,
            0.5f,
            getConfig().config["Minimum Time Needed To Be Able To Skip"].GetFloat(),
            2.5f,
            30,
            il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(classof(UnityEngine::Events::UnityAction_1<float>*), this, changeMinimumSkipTime)
        );
		QuestUI::BeatSaberUI::AddHoverHint(MinimumSkipTime->get_gameObject(), "Changes the minimum pause length in a song needed to skip the section");

        QuestUI::IncrementSetting* TimeBefore = BeatSaberUI::CreateIncrementSetting(
            Parent2,
            "Before Notes Skip To Time",
            1,
            0.5f,
            getConfig().config["How Long Before The Notes To Skip To"].GetFloat(),
            1,
            10,
            il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(classof(UnityEngine::Events::UnityAction_1<float>*), this, changeTimeBefore)
        );
		QuestUI::BeatSaberUI::AddHoverHint(TimeBefore->get_gameObject(), "Changes the time before the first notes to skip to");

        QuestUI::IncrementSetting* TextSize = BeatSaberUI::CreateIncrementSetting(
            Parent3,
            "Text Size",
            0,
            1,
            getConfig().config["Text Size"].GetInt(),
            0,
            40,
            il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(classof(UnityEngine::Events::UnityAction_1<float>*), this, changeTextSize)
        );
		QuestUI::BeatSaberUI::AddHoverHint(TextSize->get_gameObject(), "Changes the size of the text that shows when you can skip a section");
        
        QuestUI::IncrementSetting* TextXOffset = BeatSaberUI::CreateIncrementSetting(
            Parent3,
            "Text X Offset",
            0,
            1,
            getConfig().config["Text X Offset"].GetInt(),
            il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(classof(UnityEngine::Events::UnityAction_1<float>*), this, changeTextXOffset)
        );
		QuestUI::BeatSaberUI::AddHoverHint(TextXOffset->get_gameObject(), "Changes the X position of the skip text");

        QuestUI::IncrementSetting* TextYOffset = BeatSaberUI::CreateIncrementSetting(
            Parent3,
            "Text Y Offset",
            0,
            1,
            getConfig().config["Text Y Offset"].GetInt(),
            il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(classof(UnityEngine::Events::UnityAction_1<float>*), this, changeTextYOffset)
        );
		QuestUI::BeatSaberUI::AddHoverHint(TextYOffset->get_gameObject(), "Changes the Y position of the skip text");
    }
}

void IntroSkip::UIController::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)  {
    getConfig().Write();
}