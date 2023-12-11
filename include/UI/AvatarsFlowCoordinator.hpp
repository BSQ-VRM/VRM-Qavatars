#pragma once

#include "main.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include "MirrorViewController.hpp"
#include "AvatarSettingsViewController.hpp"
#include "AvatarSelectionViewController.hpp"

using namespace VRMQavatars::UI;

DECLARE_CLASS_CODEGEN(VRMQavatars::UI::FlowCoordinators, AvatarsFlowCoordinator, HMUI::FlowCoordinator,
    DECLARE_INSTANCE_FIELD(ViewControllers::MirrorViewController*, MirrorViewController);
    DECLARE_INSTANCE_FIELD(ViewControllers::AvatarSettingsViewController*, AvatarSettingsViewController);
    DECLARE_INSTANCE_FIELD(ViewControllers::AvatarSelectionViewController*, AvatarSelectionViewController);

    DECLARE_INSTANCE_METHOD(void, Awake);

    DECLARE_OVERRIDE_METHOD(void, DidActivate, GET_FIND_METHOD(&HMUI::FlowCoordinator::DidActivate), bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, BackButtonWasPressed, GET_FIND_METHOD(&HMUI::FlowCoordinator::BackButtonWasPressed), HMUI::ViewController* topViewController);
)