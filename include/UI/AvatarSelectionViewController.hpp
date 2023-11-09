#pragma once

#include "main.hpp"
#include "HMUI/ViewController.hpp"
#include "bsml/shared/macros.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "UI/components/AvatarListTableData.hpp"
#include "UI/components/AvatarListTableCell.hpp"

DECLARE_CLASS_CODEGEN(VRMQavatars::UI::ViewControllers, AvatarSelectionViewController, HMUI::ViewController,
    DECLARE_INSTANCE_FIELD(BSML::CustomListTableData*, avatarListTableData);
    DECLARE_INSTANCE_FIELD(VRMQavatars::UI::Components::AvatarListTableData*, avatarList);

    DECLARE_SIMPLE_DTOR();
    DECLARE_OVERRIDE_METHOD(void, DidActivate, GET_FIND_METHOD(&HMUI::ViewController::DidActivate), bool firstActivation, bool addedToHeirarchy, bool screenSystemDisabling);
    DECLARE_INSTANCE_METHOD(void, Refresh);
    DECLARE_INSTANCE_METHOD(void, Calibrate);
    DECLARE_INSTANCE_METHOD(void, PostParse);
    DECLARE_INSTANCE_METHOD(void, OnSelectAvatar, VRMQavatars::UI::Components::AvatarListTableCell* cell);
    DECLARE_INSTANCE_METHOD(int, GetSelectedCellIdx);
)