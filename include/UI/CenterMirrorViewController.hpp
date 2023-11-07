#pragma once

#include "main.hpp"
#include "HMUI/ViewController.hpp"
#include "bsml/shared/macros.hpp"
#include "bsml/shared/BSML.hpp"

DECLARE_CLASS_CODEGEN(VRMQavatars::UI::ViewControllers, CenterMirrorViewController, HMUI::ViewController,
    DECLARE_SIMPLE_DTOR();
    DECLARE_OVERRIDE_METHOD(void, DidActivate, GET_FIND_METHOD(&HMUI::ViewController::DidActivate), bool firstActivation, bool addedToHeirarchy, bool screenSystemDisabling);
)