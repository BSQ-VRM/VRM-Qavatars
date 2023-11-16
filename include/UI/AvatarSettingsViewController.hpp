#pragma once

#include "main.hpp"
#include "HMUI/ViewController.hpp"
#include "bsml/shared/BSML.hpp"

#include "utils/structs/OffsetPose.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/XUI/XUI.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK_BS/UI/ViewController.hpp"
#include "modals/IndividualConfigModal.hpp"

namespace VRMQavatars::UI::ViewControllers {
    class AvatarSettingsViewController : public CP_SDK_BS::UI::ViewController {
        CP_SDK_IL2CPP_INHERIT("QBeatSaberPlus_Multiplayer.UI", AvatarSettingsViewController, CP_SDK_BS::UI::ViewController);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(AvatarSettingsViewController);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(AvatarSettingsViewController);
        SDK_UI_VIEW_CONTROLLER_INSTANCE();
    public:
        void DidActivate();
        void UpdatePos();

        CP_SDK::Utils::MonoPtr<Modals::IndividualConfigModal> configModal;

        UnityEngine::Vector3 lightRotation = {30.0f, 240.0f, 0.0f};

        std::shared_ptr<CP_SDK::XUI::XUISlider> BuildFingerSlider(int finger);

        std::shared_ptr<CP_SDK::XUI::XUIVLayout> BuildCalibrationTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildHandOffsetsTab();
        std::shared_ptr<CP_SDK::XUI::XUITabControl> BuildFaceTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildIKSettingsTab();
        std::shared_ptr<CP_SDK::XUI::XUITabControl> BuildFingerPoseSettingsTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildLocoSettingsTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildLightingTab();

        //Hand Offsets

        void UpdateHandOffsetsTab();

        std::shared_ptr<CP_SDK::XUI::XUISlider> xPosSlider;
        std::shared_ptr<CP_SDK::XUI::XUISlider> yPosSlider;
        std::shared_ptr<CP_SDK::XUI::XUISlider> zPosSlider;

        std::shared_ptr<CP_SDK::XUI::XUISlider> xRotSlider;
        std::shared_ptr<CP_SDK::XUI::XUISlider> yRotSlider;
        std::shared_ptr<CP_SDK::XUI::XUISlider> zRotSlider;

        std::shared_ptr<CP_SDK::XUI::XUIToggle> handOffsetsGlobalToggle;

        //Finger Posing

        void UpdateFingerPosingTab();

        std::vector<std::shared_ptr<CP_SDK::XUI::XUISlider>> fingerSliders = std::vector<std::shared_ptr<CP_SDK::XUI::XUISlider>>(20);

        //Calibration
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::UI::ViewControllers::AvatarSettingsViewController);