#include "UI/modals/IndividualConfigModal.hpp"

#include "AvatarManager.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/XUI/Templates.hpp"
#include "config/ConfigManager.hpp"
#include "UI/AvatarSelectionViewController.hpp"
#include "UI/AvatarSettingsViewController.hpp"

namespace VRMQavatars::UI::Modals {

    CP_SDK_IL2CPP_INHERIT_INIT(IndividualConfigModal);
    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(IndividualConfigModal)
    {
        OnShow  = {this, &IndividualConfigModal::Impl_OnShow};
        OnClose = {this, &IndividualConfigModal::Impl_OnClose};
    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(IndividualConfigModal)
    {

    }

    void IndividualConfigModal::Impl_OnShow()
    {
        if(offsetToggle != nullptr)
        {
            offsetToggle->SetInteractable(AvatarManager::currentContext != nullptr);
            fingerToggle->SetInteractable(AvatarManager::currentContext != nullptr);
            ikToggle->SetInteractable(AvatarManager::currentContext != nullptr);
            locoToggle->SetInteractable(AvatarManager::currentContext != nullptr);
            lightToggle->SetInteractable(AvatarManager::currentContext != nullptr);
            triggerToggle->SetInteractable(AvatarManager::currentContext != nullptr);
            return;
        }
        CP_SDK::XUI::Templates::ModalRectLayout({
            CP_SDK::XUI::XUIText::Make(u"For this avatar..."),
            CP_SDK::XUI::XUIVLayout::Make({
                CP_SDK::XUI::XUIHLayout::Make({
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIText::Make(u"Use Individual Offsets"),
                        CP_SDK::XUI::XUIToggle::Make()
                            ->SetValue(AvatarManager::currentContext != nullptr ? Config::ConfigManager::GetAvatarConfig().OverrideOffsetSettings.GetValue() : false)
                            ->SetInteractable(AvatarManager::currentContext != nullptr)
                            ->Bind(&offsetToggle)
                            ->OnValueChanged(CP_SDK::Utils::Action<bool>([this](const bool val)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideOffsetSettings.SetValue(val);
                                VController->GetComponent<ViewControllers::AvatarSettingsViewController*>()->UpdateHandOffsetsTab();
                            }))
                            ->AsShared()
                    }),
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIText::Make(u"Use Individual Finger Poses"),
                        CP_SDK::XUI::XUIToggle::Make()
                            ->SetValue(AvatarManager::currentContext != nullptr ? Config::ConfigManager::GetAvatarConfig().OverrideFingerPoseSettings.GetValue() : false)
                            ->SetInteractable(AvatarManager::currentContext != nullptr)
                            ->Bind(&fingerToggle)
                            ->OnValueChanged(CP_SDK::Utils::Action<bool>([this](const bool val)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideFingerPoseSettings.SetValue(val);
                                VController->GetComponent<ViewControllers::AvatarSettingsViewController*>()->UpdateFingerPosingTab();
                            }))
                            ->AsShared()
                    }),
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIText::Make(u"Use Individual IK Settings"),
                        CP_SDK::XUI::XUIToggle::Make()
                            ->SetValue(AvatarManager::currentContext != nullptr ? Config::ConfigManager::GetAvatarConfig().OverrideIKSettings.GetValue() : false)
                            ->SetInteractable(AvatarManager::currentContext != nullptr)
                            ->Bind(&ikToggle)
                            ->OnValueChanged(CP_SDK::Utils::Action<bool>([this](const bool val)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideIKSettings.SetValue(val);
                                VController->GetComponent<ViewControllers::AvatarSettingsViewController*>()->UpdateIKTab();
                            }))
                            ->AsShared()
                    }),
                }),
                CP_SDK::XUI::XUIHLayout::Make({
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIText::Make(u"Use Individual Locomotion Settings"),
                        CP_SDK::XUI::XUIToggle::Make()
                            ->SetValue(AvatarManager::currentContext != nullptr ? Config::ConfigManager::GetAvatarConfig().OverrideOffsetSettings.GetValue() : false)
                            ->SetInteractable(AvatarManager::currentContext != nullptr)
                            ->Bind(&locoToggle)
                            ->OnValueChanged(CP_SDK::Utils::Action<bool>([this](const bool val)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideLocomotionSettings.SetValue(val);
                                VController->GetComponent<ViewControllers::AvatarSettingsViewController*>()->UpdateHandOffsetsTab();
                            }))
                            ->AsShared()
                    }),
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIText::Make(u"Use Individual Lighting Settings"),
                        CP_SDK::XUI::XUIToggle::Make()
                            ->SetValue(AvatarManager::currentContext != nullptr ? Config::ConfigManager::GetAvatarConfig().OverrideFingerPoseSettings.GetValue() : false)
                            ->SetInteractable(AvatarManager::currentContext != nullptr)
                            ->Bind(&lightToggle)
                            ->OnValueChanged(CP_SDK::Utils::Action<bool>([this](const bool val)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideLightingSettings.SetValue(val);
                                VController->GetComponent<ViewControllers::AvatarSettingsViewController*>()->UpdateFingerPosingTab();
                            }))
                            ->AsShared()
                    }),
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIText::Make(u"Use Individual Controller Trigger Settings"),
                        CP_SDK::XUI::XUIToggle::Make()
                            ->SetValue(AvatarManager::currentContext != nullptr ? Config::ConfigManager::GetAvatarConfig().OverrideIKSettings.GetValue() : false)
                            ->SetInteractable(AvatarManager::currentContext != nullptr)
                            ->Bind(&triggerToggle)
                            ->OnValueChanged(CP_SDK::Utils::Action<bool>([this](const bool val)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideControllerTriggerSettings.SetValue(val);
                                VController->GetComponent<ViewControllers::AvatarSettingsViewController*>()->UpdateIKTab();
                            }))
                            ->AsShared()
                    }),
                }),
            }),
            CP_SDK::XUI::XUIPrimaryButton::Make(u"Close")
                ->OnClick(CP_SDK::Utils::Action<>([this]
                {
                    VController->CloseModal(this);
                }))
                ->AsShared()
        })
        ->BuildUI(get_transform());
    }

    void IndividualConfigModal::Impl_OnClose()
    {

    }
}
