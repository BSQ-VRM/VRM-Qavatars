#pragma once

#include "main.hpp"
#include "HMUI/ViewController.hpp"
#include "bsml/shared/BSML.hpp"

#include "utils/structs/OffsetPose.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/XUI/XUI.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK_BS/UI/ViewController.hpp"

namespace VRMQavatars::UI::ViewControllers {
    class AvatarSettingsViewController : public CP_SDK_BS::UI::ViewController {
        CP_SDK_IL2CPP_INHERIT("QBeatSaberPlus_Multiplayer.UI", AvatarSettingsViewController, CP_SDK_BS::UI::ViewController);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(AvatarSettingsViewController);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(AvatarSettingsViewController);
        SDK_UI_VIEW_CONTROLLER_INSTANCE();
    public:
        void DidActivate();
        void UpdatePos();

        Structs::OffsetPose offsetPose;

        UnityEngine::Vector3 lightRotation = {30.0f, 240.0f, 0.0f};

        std::string pose = "-34,-66,-80,0,-34,-66,-65,0,-37,-73,-43,0,-46,-48,-30,0,-77,-47,-20,19";

        std::shared_ptr<CP_SDK::XUI::XUIVLayout> BuildCalibrationTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildHandOffsetsTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildIKSettingsTab();
        std::shared_ptr<CP_SDK::XUI::XUITabControl> BuildFingerPoseSettingsTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildLocoSettingsTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildLightingTab();
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::UI::ViewControllers::AvatarSettingsViewController);