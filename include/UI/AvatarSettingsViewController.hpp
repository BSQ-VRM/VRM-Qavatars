#pragma once

#include "main.hpp"
#include "HMUI/ViewController.hpp"
#include "bsml/shared/macros.hpp"
#include "bsml/shared/BSML.hpp"

DECLARE_CLASS_CODEGEN(VRMQavatars::UI::ViewControllers, AvatarSettingsViewController, HMUI::ViewController,
    DECLARE_SIMPLE_DTOR();
    DECLARE_OVERRIDE_METHOD(void, DidActivate, GET_FIND_METHOD(&HMUI::ViewController::DidActivate), bool firstActivation, bool addedToHeirarchy, bool screenSystemDisabling);

    DECLARE_INSTANCE_METHOD(void, UpdatePos);

    DECLARE_BSML_PROPERTY(float, leftHandRotX);
    DECLARE_BSML_PROPERTY(float, leftHandRotY);
    DECLARE_BSML_PROPERTY(float, leftHandRotZ);
    
    DECLARE_BSML_PROPERTY(float, leftHandPosX);
    DECLARE_BSML_PROPERTY(float, leftHandPosY);
    DECLARE_BSML_PROPERTY(float, leftHandPosZ);
)