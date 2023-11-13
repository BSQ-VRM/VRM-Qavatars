#pragma once

#include "main.hpp"
#include "HMUI/ViewController.hpp"
#include "bsml/shared/macros.hpp"
#include "bsml/shared/BSML.hpp"

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

        float handOffsetX;
        float handOffsetY;
        float handOffsetZ;

        float handRotX;
        float handRotY;
        float handRotZ;


        std::shared_ptr<CP_SDK::XUI::XUIVLayout> BuildCalibrationTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildHandOffsetsTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildIKSettingsTab();
        std::shared_ptr<CP_SDK::XUI::XUIHLayout> BuildLightingTab();
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::UI::ViewControllers::AvatarSettingsViewController);