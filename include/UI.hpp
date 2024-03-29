#pragma once

#include "main.hpp"
#include "HMUI/ViewController.hpp"

static ModInfo modInfo;

DECLARE_CLASS_CODEGEN(IntroSkip, UIController, HMUI::ViewController,
    
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, DidDeactivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidDeactivate", 2), bool removedFromHierarchy, bool systemScreenDisabling);
)