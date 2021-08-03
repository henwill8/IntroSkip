#include "../include/UI.hpp"
#include "../include/codegen.hpp"
using namespace QuestUI;
using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(IntroSkip, UIController);

#define CreateIncrementMacro(parent, floatConfigValue, name, decimals, increment, hasMin, hasMax, minValue, maxValue) QuestUI::BeatSaberUI::CreateIncrementSetting(parent, name, decimals, increment, floatConfigValue.GetFloat(), hasMin, hasMax, minValue, maxValue, UnityEngine::Vector2{}, [this](float value) { floatConfigValue.SetFloat(value); })

#define CreateToggleMacro(parent, boolConfigValue, name) QuestUI::BeatSaberUI::CreateToggle(parent, name, boolConfigValue.GetBool(), [this](bool newValue) { boolConfigValue.SetBool(newValue);})

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

		QuestUI::BeatSaberUI::AddHoverHint(CreateToggleMacro(Parent1, getConfig().config["Are Intro Skips Enabled"], "Enable Intro Skips")->get_gameObject(), "Songs without notes during the intro will be skippable when enabled");
		QuestUI::BeatSaberUI::AddHoverHint(CreateToggleMacro(Parent1, getConfig().config["Are Middle Skips Enabled"], "Enable Middle Skips")->get_gameObject(), "Pauses in the middle of songs will be skippable when enabled");
		QuestUI::BeatSaberUI::AddHoverHint(CreateToggleMacro(Parent1, getConfig().config["Are Outro Skips Enabled"], "Enable Outro Skips")->get_gameObject(), "Songs without notes during the outro will be skippable when enabled");

		QuestUI::BeatSaberUI::AddHoverHint(CreateIncrementMacro(Parent2, getConfig().config["Time Held To Skip"], "Hold Time", 1, 0.1f, true, true, 0, 5)->get_gameObject(), "Changes the amount of time you have to hold the triggers to skip a section, setting to 0 will cause automatic skipping");
		QuestUI::BeatSaberUI::AddHoverHint(CreateIncrementMacro(Parent2, getConfig().config["Minimum Time Needed To Be Able To Skip"], "Minimum Skip Time", 1, 0.5f, true, true, 2.5f, 30)->get_gameObject(), "Changes the minimum pause length in a song needed to skip the section");
		QuestUI::BeatSaberUI::AddHoverHint(CreateIncrementMacro(Parent2, getConfig().config["How Long Before Notes To Skip To"], "Before Notes Skip To Time", 1, 0.5f, true, true, 1, 10)->get_gameObject(), "Changes the time before the first notes to skip to");

		QuestUI::BeatSaberUI::AddHoverHint(CreateIncrementMacro(Parent3, getConfig().config["Text Size"], "Text Size", 0, 1, true, true, 0, 40)->get_gameObject(), "Changes the size of the text that shows when you can skip a section");
		QuestUI::BeatSaberUI::AddHoverHint(CreateIncrementMacro(Parent3, getConfig().config["Text X Offset"], "Text X Offset", 0, 1, false, false, 0, 0)->get_gameObject(), "Changes the X position of the skip text");
		QuestUI::BeatSaberUI::AddHoverHint(CreateIncrementMacro(Parent3, getConfig().config["Text Y Offset"], "Text Y Offset", 0, 1, false, false, 0, 0)->get_gameObject(), "Changes the Y position of the skip text");
    }
}

void IntroSkip::UIController::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)  {
    getConfig().Write();
}