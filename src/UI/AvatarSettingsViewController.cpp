#include "UI/AvatarSettingsViewController.hpp"

#include <HandController.hpp>
#include <sstream>

#include "LightManager.hpp"

#include "assets.hpp"
#include "AvatarManager.hpp"

#include <string_view>
#include <bsml/shared/Helpers/utilities.hpp>
#include <UnityEngine/Resources.hpp>

#include "MirrorManager.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/XUI/Templates.hpp"
#include "config/ConfigManager.hpp"
#include "customTypes/VRMWind.hpp"
#include "customTypes/BlendShape/BlendShapeController.hpp"
#include "UI/components/AvatarListCell.hpp"
#include "VMC/VMCClient.hpp"
#include "VMC/VMCServer.hpp"

namespace VRMQavatars::UI::ViewControllers {
    CP_SDK_IL2CPP_INHERIT_INIT(AvatarSettingsViewController);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(AvatarSettingsViewController)
    {
        OnViewCreation      = {this, &AvatarSettingsViewController::DidActivate};
        OnViewActivation    = {this, &AvatarSettingsViewController::OnShow};
        OnViewDeactivation  = {this, &AvatarSettingsViewController::DidDeactivate};
    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(AvatarSettingsViewController)
    {

    }

    UnityEngine::Sprite* GetBGSprite(std::string str)
    {
        return UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Sprite*>().First( [str](UnityEngine::Sprite* x) { return x->get_name() == str; });
    }

    UnityEngine::Material* GetBGMat(std::string str)
    {
        return UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>().First([str](UnityEngine::Material* x) { return x->get_name() == str; });
    }

    void AvatarSettingsViewController::DidActivate()
    {
        globalSprite = BSML::Utilities::LoadSpriteRaw(IncludedAssets::global_png);
        avatarSprite = BSML::Utilities::LoadSpriteRaw(IncludedAssets::avatar_png);

        CP_SDK::XUI::Templates::FullRectLayout({
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUIIconButton::Make(globalSprite)
                    ->Bind(&overrideSwitchButton)
                    ->OnClick([this]
                    {
                        if(selectedTab == "Offset")
                        {
                            const auto override = Config::ConfigManager::GetOffsetOverride();
                            if(AvatarManager::currentContext != nullptr)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideOffsetSettings.SetValue(!override);
                            }
                            AvatarManager::SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
                            UpdateHandOffsetsTab();
                            goto refresh;
                        }
                        if(selectedTab == "Finger Posing")
                        {
                            const auto override = Config::ConfigManager::GetFingerPoseOverride();
                            if(AvatarManager::currentContext != nullptr)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideFingerPoseSettings.SetValue(!override);
                            }
                            AvatarManager::SetFingerPose(Config::ConfigManager::GetFingerPoseSettings().grabPose);
                            UpdateFingerPosingTab();
                            goto refresh;
                        }
                        if(selectedTab == "BlendShapes")
                        {
                            const auto override = Config::ConfigManager::GetBlendshapeOverride();
                            if(AvatarManager::currentContext != nullptr)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideBlendshapeSettings.SetValue(!override);
                            }
                            UpdateBlendshapesTab();
                            goto refresh;
                        }
                        if(selectedTab == "IK")
                        {
                            const auto override = Config::ConfigManager::GetIKOverride();
                            if(AvatarManager::currentContext != nullptr)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideIKSettings.SetValue(!override);
                            }
                            AvatarManager::UpdateVRIK();
                            UpdateIKTab();
                            goto refresh;
                        }
                        if(selectedTab == "Locomotion")
                        {
                            const auto override = Config::ConfigManager::GetLocomotionOverride();
                            if(AvatarManager::currentContext != nullptr)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideLocomotionSettings.SetValue(!override);
                            }
                            AvatarManager::UpdateVRIK();
                            UpdateLocomotionTab();
                            goto refresh;
                        }
                        if(selectedTab == "Lighting")
                        {
                            const auto override = Config::ConfigManager::GetLightingOverride();
                            if(AvatarManager::currentContext != nullptr)
                            {
                                Config::ConfigManager::GetAvatarConfig().OverrideLightingSettings.SetValue(!override);
                            }
                            LightManager::UpdateLightValues();
                            UpdateLightingTab();
                        }
                        refresh:
                        RefreshButton();
                    })
                    ->SetWidth(6.0f)
                    ->SetHeight(6.0f)
                    ->AsShared(),
                CP_SDK::XUI::Templates::TitleBar(u"Avatar Settings")->AsShared(),
            }),
            CP_SDK::XUI::XUITabControl::Make(u"Settings Tab", {
                { u"Calibration", BuildCalibrationTab() },
                { u"Hands", CP_SDK::XUI::XUITabControl::Make(
                    {
                        { u"Offset", BuildHandOffsetsTab() }, { u"Finger Posing", BuildFingerPoseSettingsTab() }
                    })
                    ->OnActiveChanged([this](const int tab)
                    {
                        const std::vector opts = { "Offset", "Finger Posing" };
                        handTab = opts[tab];
                        selectedTab = handTab;
                        TabSelected();
                    })
                    ->AsShared()
                },
                { u"Face", BuildFaceTab() },
                { u"VMC", BuildVMCTab() },
                { u"VRIK", CP_SDK::XUI::XUITabControl::Make(
                    {
                        { u"IK", BuildIKSettingsTab() }, { u"Locomotion", BuildLocoSettingsTab() }
                    })
                    ->OnActiveChanged([this](const int tab)
                    {
                        const std::vector opts = { "IK", "Locomotion" };
                        ikTab = opts[tab];
                        selectedTab = ikTab;
                        TabSelected();
                    })
                    ->AsShared()
                },
                { u"Mirror", BuildMirrorTab() },
                { u"Lighting", BuildLightingTab() },
                { u"Wind", BuildWindTab() },
            })
            ->OnActiveChanged([this](const int tab)
            {
                const std::vector opts = { "Calibration", "Hands", "Face", "VMC", "VRIK", "Mirror", "Lighting", "Wind" };
                selectedTab = opts[tab];
                if(selectedTab == "Hands")
                    selectedTab = handTab;
                if(selectedTab == "VRIK")
                    selectedTab = ikTab;
                if(selectedTab == "Face")
                    selectedTab = faceTab;
                TabSelected();
            })
            ->AsShared()
        })
        ->SetSpacing(1.0f)
        ->BuildUI(get_transform());

        AvatarManager::OnLoad += [this]
        {
            UpdateIKTab();
            UpdateHandOffsetsTab();
            UpdateFingerPosingTab();
            UpdateControllerTriggerTab();
            UpdateLocomotionTab();
            UpdateLightingTab();
            UpdateBlendshapesTab();
            UpdateCalibrationTab();
        };

        if(AvatarManager::currentContext != nullptr)
        {
            UpdateBlendshapesTab();
            UpdateControllerTriggerTab();
        }

        /*helperScreen = BSML::FloatingScreen::CreateFloatingScreen({30, 22}, true, {-1.0f, 0.3f, 1.0f}, UnityEngine::Quaternion::Euler({45, -45, 0}), 0.0f, false);
        UnityEngine::GameObject::DontDestroyOnLoad(helperScreen);

        CP_SDK::XUI::Templates::ModalRectLayout({
            CP_SDK::XUI::XUIVLayout::Make({
                CP_SDK::XUI::XUIText::Make(u"Helper"),
                CP_SDK::XUI::XUIHLayout::Make({
                    CP_SDK::XUI::XUISecondaryButton::Make(u"Reset Active Tab!")
                        ->Bind(&resetButton)
                        ->OnClick([this]
                        {
                            if(selectedTab == "Offset")
                            {
                                Config::ConfigManager::ResetOffsetSettings();
                                UpdateHandOffsetsTab();
                                AvatarManager::SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
                            }
                            if(selectedTab == "Locomotion")
                            {
                                Config::ConfigManager::ResetLocomotionSettings();
                                UpdateLocomotionTab();
                                AvatarManager::UpdateVRIK();
                            }
                            if(selectedTab == "Lighting")
                            {
                                Config::ConfigManager::ResetLightingSettings();
                                UpdateLightingTab();
                            }
                            if(selectedTab == "Face")
                            {
                                Config::ConfigManager::ResetBlendshapeSettings();
                                UpdateBlendshapesTab();
                            }
                            if(selectedTab == "Controller Triggers")
                            {
                                Config::ConfigManager::ResetControllerTriggerSettings();
                                UpdateControllerTriggerTab();
                            }
                            if(selectedTab == "IK")
                            {
                                Config::ConfigManager::ResetIKSettings();
                                UpdateLocomotionTab();
                                AvatarManager::UpdateVRIK();
                            }
                            if(selectedTab == "Wind")
                            {
                                Config::ConfigManager::ResetWindSettings();
                                UpdateWindTab();
                            }
                            if(selectedTab == "Finger Posing")
                            {
                                Config::ConfigManager::ResetFingerPoseSettings();
                                UpdateFingerPosingTab();
                                AvatarManager::SetFingerPose(Config::ConfigManager::GetFingerPoseSettings().grabPose);
                            }
                        })
                        ->AsShared()
                })
            })
        })
        ->BuildUI(helperScreen->get_transform());*/

        TabSelected();
    }

    void AvatarSettingsViewController::DidDeactivate()
    {
        //helperScreen->get_gameObject()->SetActive(false);
    }

    void AvatarSettingsViewController::OnShow()
    {
        /*if(helperScreen != nullptr)
        {
            helperScreen->get_gameObject()->SetActive(true);
        }

        globalSprite = QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::global_png);
        avatarSprite = QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::avatar_png);*/
        RefreshButton();
    }

    void AvatarSettingsViewController::TabSelected()
    {
        RefreshButton();
        //resetButton->SetInteractable(selectedTab != "VMC" && selectedTab != "Mirror" && selectedTab != "Calibration");
    }

    void AvatarSettingsViewController::RefreshButton()
    {
        globalSprite = BSML::Utilities::LoadSpriteRaw(IncludedAssets::global_png);
        avatarSprite = BSML::Utilities::LoadSpriteRaw(IncludedAssets::avatar_png);

        bool interactable = false;
        UnityEngine::Sprite* toUse = BSML::Utilities::LoadSpriteRaw(IncludedAssets::none_png);
        if(selectedTab == "Calibration")
        {
            interactable = false;
            toUse = globalSprite;
            goto setButton;
        }
        if(selectedTab == "Offset")
        {
            interactable = true;
            toUse = Config::ConfigManager::GetOffsetOverride() ? avatarSprite : globalSprite;
            goto setButton;
        }
        if(selectedTab == "Finger Posing")
        {
            interactable = true;
            toUse = Config::ConfigManager::GetFingerPoseOverride() ? avatarSprite : globalSprite;
            goto setButton;
        }
        if(selectedTab == "BlendShapes")
        {
            interactable = true;
            toUse = Config::ConfigManager::GetBlendshapeOverride() ? avatarSprite : globalSprite;
            goto setButton;
        }
        if(selectedTab == "Controller Expressions")
        {
            interactable = false;
            toUse = avatarSprite;
            goto setButton;
        }
        if(selectedTab == "VMC")
        {
            interactable = false;
            toUse = globalSprite;
            goto setButton;
        }
        if(selectedTab == "IK")
        {
            interactable = true;
            toUse = Config::ConfigManager::GetIKOverride() ? avatarSprite : globalSprite;
            goto setButton;
        }
        if(selectedTab == "Locomotion")
        {
            interactable = true;
            toUse = Config::ConfigManager::GetLocomotionOverride() ? avatarSprite : globalSprite;
            goto setButton;
        }
        if(selectedTab == "Lighting")
        {
            interactable = true;
            toUse = Config::ConfigManager::GetLightingOverride() ? avatarSprite : globalSprite;
            goto setButton;
        }
        if(selectedTab == "Mirror")
        {
            interactable = false;
            toUse = globalSprite;
            goto setButton;
        }
        if(selectedTab == "Wind")
        {
            interactable = false;
            toUse = globalSprite;
        }
        setButton:
        overrideSwitchButton->SetInteractable(interactable);
        overrideSwitchButton->SetSprite(toUse);
    }

    std::shared_ptr<CP_SDK::XUI::XUIVLayout> AvatarSettingsViewController::BuildCalibrationTab()
    {
        std::vector<std::u16string> calibOptions = { u"Match Armspans", u"Match Heights", u"Fixed" };
        return CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"Calibration Type"),
                CP_SDK::XUI::XUIDropdown::Make()
                    ->SetOptions(calibOptions)
                    ->OnValueChanged([this](const int idx, const std::u16string_view val)
                    {
                        fixedSlider->SetInteractable(val == u"Fixed");
                        Config::ConfigManager::GetGlobalConfig().CalibrationType.SetValue(idx);
                        Config::ConfigManager::GetGlobalConfig().Save();
                    })
                    ->OnReady([calibOptions](CP_SDK::UI::Components::CDropdown* dropdown)
                    {
                        dropdown->SetValue(calibOptions[Config::ConfigManager::GetGlobalConfig().CalibrationType.GetValue()]);
                    })
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Fixed Scale"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(0.2f)
                    ->SetMaxValue(2.0f)
                    ->SetValue(Config::ConfigManager::GetGlobalConfig().FixedScale.GetValue())
                    ->OnValueChanged([](const float val)
                    {
                        Config::ConfigManager::GetGlobalConfig().FixedScale.SetValue(val);
                        Config::ConfigManager::GetGlobalConfig().Save();
                    })
                    ->SetIncrements(18.0f)
                    ->SetInteractable(Config::ConfigManager::GetGlobalConfig().CalibrationType.GetValue() == 2)
                    ->Bind(&fixedSlider)
                ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"The following options require recalibration!"),

                CP_SDK::XUI::XUIHLayout::Make(
                {
                    CP_SDK::XUI::XUIText::Make(u"Arm Scale (Per avatar)"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetMinValue(0.2f)
                        ->SetMaxValue(2.0f)
                        ->SetIncrements(180)
                        ->SetValue(AvatarManager::currentContext != nullptr ? Config::ConfigManager::GetAvatarConfig().ArmCalibrationScale.GetValue() : 1.0f)
                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                        ->OnValueChanged([](const float val)
                        {
                            if(AvatarManager::currentContext != nullptr)
                            {
                                Config::ConfigManager::GetAvatarConfig().ArmCalibrationScale.SetValue(val);
                                Config::ConfigManager::SaveAvatarConfig();
                            }
                        })
                        ->Bind(&armScaleSlider)
                        ->AsShared(),
                }),
                /*CP_SDK::XUI::XUIHLayout::Make(
                {
                    CP_SDK::XUI::XUIText::Make(u"Leg Scale (Per avatar)"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetMinValue(0.75f)
                        ->SetMaxValue(1.25f)
                        ->SetValue(AvatarManager::currentContext != nullptr ? Config::ConfigManager::GetAvatarConfig().LegCalibrationScale.GetValue() : 1.0f)
                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                        ->OnValueChanged([](const float val)
                        {
                            Config::ConfigManager::GetAvatarConfig().LegCalibrationScale.SetValue(val);
                            Config::ConfigManager::SaveAvatarConfig();
                        })
                        ->Bind(&legScaleSlider)
                        ->AsShared()
                })*/
            }
        );
    }

    void AvatarSettingsViewController::UpdateCalibrationTab()
    {
        armScaleSlider->SetValue(AvatarManager::currentContext != nullptr ? Config::ConfigManager::GetAvatarConfig().ArmCalibrationScale.GetValue() : 1.0f);
        armScaleSlider->SetInteractable(AvatarManager::currentContext != nullptr);

        //legScaleSlider->SetValue(AvatarManager::currentContext != nullptr ? Config::ConfigManager::GetAvatarConfig().LegCalibrationScale.GetValue() : 1.0f);
        //legScaleSlider->SetInteractable(AvatarManager::currentContext != nullptr);
    }

    std::shared_ptr<CP_SDK::XUI::XUIHLayout> AvatarSettingsViewController::BuildHandOffsetsTab()
    {
        return CP_SDK::XUI::XUIHLayout::Make(
            {
                CP_SDK::XUI::XUIVLayout::Make(
                {
                    CP_SDK::XUI::Templates::TitleBar(u"Position")
                        ->SetBackground(true, UnityEngine::Color(0.2f, 0.4f, 0.61f, 1.0f), true)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"X Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({1.0f, 0.0f, 0.0f, 1.0f})
                        ->SetIncrements(20.0f)
                        ->SetMinValue(-0.5f)
                        ->SetMaxValue(0.5f)
                        ->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.posX)
                        ->OnValueChanged([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.posX = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            AvatarManager::SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
                        })
                        ->Bind(&xPosSlider)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Y Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 1.0f, 0.0f, 1.0f})
                        ->SetIncrements(20.0f)
                        ->SetMinValue(-0.5f)
                        ->SetMaxValue(0.5f)
                        ->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.posY)
                        ->OnValueChanged([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.posY = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            AvatarManager::SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
                        })
                        ->Bind(&yPosSlider)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Z Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 0.0f, 1.0f, 1.0f})
                        ->SetIncrements(20.0f)
                        ->SetMinValue(-0.5f)
                        ->SetMaxValue(0.5f)
                        ->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.posZ)
                        ->OnValueChanged([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.posZ = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            AvatarManager::SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
                        })
                        ->Bind(&zPosSlider)
                        ->AsShared(),
                })
                ->SetSpacing(-0.05f)
                ->AsShared(),

                CP_SDK::XUI::XUIVLayout::Make(
                {
                    CP_SDK::XUI::Templates::TitleBar(u"Rotation")
                        ->SetBackground(true, UnityEngine::Color(0.56f, 0.05f, 0.25f, 1.0f), true)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"X Rotation"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({1.0f, 0.0f, 0.0f, 1.0f})
                        ->SetIncrements(180.0f)
                        ->SetMinValue(-90.0f)
                        ->SetMaxValue(90.0f)
                        ->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.rotX)
                        ->OnValueChanged([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.rotX = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            AvatarManager::SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
                        })
                        ->Bind(&xRotSlider)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Y Rotation"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 1.0f, 0.0f, 1.0f})
                        ->SetIncrements(180.0f)
                        ->SetMinValue(-90.0f)
                        ->SetMaxValue(90.0f)
                        ->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.rotY)
                        ->OnValueChanged([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.rotY = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            AvatarManager::SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
                        })
                        ->Bind(&yRotSlider)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Z Rotation"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 0.0f, 1.0f, 1.0f})
                        ->SetIncrements(180.0f)
                        ->SetMinValue(-90.0f)
                        ->SetMaxValue(90.0f)
                        ->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.rotZ)
                        ->OnValueChanged([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.rotZ = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            AvatarManager::SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
                        })
                        ->Bind(&zRotSlider)
                        ->AsShared()
                })
                ->SetSpacing(-0.05f)
                ->AsShared()
            })
            ->SetSpacing(10.0f)
            ->AsShared();
    }

    void AvatarSettingsViewController::UpdateHandOffsetsTab()
    {
        xPosSlider->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.posX);
        yPosSlider->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.posY);
        zPosSlider->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.posZ);
        xRotSlider->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.rotX);
        yRotSlider->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.rotY);
        zRotSlider->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.rotZ);
    }

    std::shared_ptr<CP_SDK::XUI::XUITabControl> AvatarSettingsViewController::BuildFaceTab()
    {
        return CP_SDK::XUI::XUITabControl::Make({
                {
                    u"BlendShapes",
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIHLayout::Make({
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Auto Blink"),
                                CP_SDK::XUI::XUIToggle::Make()
                                    ->Bind(&autoBlinkToggle)
                                    ->OnValueChanged([](const bool val) {
                                        auto settings = Config::ConfigManager::GetBlendshapeSettings();
                                        settings.autoBlink = val;
                                        Config::ConfigManager::SetBlendshapeSettings(settings);
                                    })
                                    ->AsShared(),
                                CP_SDK::XUI::XUIText::Make(u"Blink Wait Time"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->Bind(&autoBlinkWaitSlider)
                                    ->SetMaxValue(4.0f)
                                    ->OnValueChanged([](const float val) {
                                        auto settings = Config::ConfigManager::GetBlendshapeSettings();
                                        settings.waitTime = val;
                                        Config::ConfigManager::SetBlendshapeSettings(settings);
                                    })
                                    ->AsShared(),
                            })
                            ->SetSpacing(0.1f)
                            ->AsShared(),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Mock Eye Movement"),
                                CP_SDK::XUI::XUIToggle::Make()
                                    ->Bind(&mockEyeMovementToggle)
                                    ->OnValueChanged([](const bool val) {
                                        auto settings = Config::ConfigManager::GetBlendshapeSettings();
                                        settings.mockEyeMovement = val;
                                        Config::ConfigManager::SetBlendshapeSettings(settings);
                                    })
                                    ->AsShared(),
                                CP_SDK::XUI::XUIText::Make(u"Default Facial Expression"),
                                CP_SDK::XUI::XUIDropdown::Make()
                                    ->Bind(&neutralExpressionDropdown)
                                    ->SetInteractable(AvatarManager::currentContext != nullptr)
                                    ->OnValueChanged([](const int idx, const std::u16string_view val) {
                                        auto settings = Config::ConfigManager::GetBlendshapeSettings();
                                        settings.neutralExpression = to_utf8(val);
                                        Config::ConfigManager::SetBlendshapeSettings(settings);
                                    })
                                    ->AsShared(),
                            })
                            ->SetSpacing(0.1f)
                            ->AsShared()
                        })
                    })
                },
                /*{
                    u"Ingame Event Expressions",
                    CP_SDK::XUI::XUITabControl::Make({
                        {
                            u"Combo",
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Event"),
                                    CP_SDK::XUI::XUIText::Make(u"Condition"),
                                    CP_SDK::XUI::XUIText::Make(u"BlendShape"),
                                    CP_SDK::XUI::XUIText::Make(u"Duration")
                                })
                                ->SetSpacing(25.0f)
                                ->AsShared(),
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"On Combo Reached"),
                                    CP_SDK::XUI::XUIDropdown::Make()
                                    CP_SDK::XUI::XUIDropdown::Make(),
                                    CP_SDK::XUI::XUIDropdown::Make()
                                }),
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Every X Combo"),
                                    CP_SDK::XUI::XUIDropdown::Make(),
                                    CP_SDK::XUI::XUIDropdown::Make(),
                                    CP_SDK::XUI::XUIDropdown::Make()
                                }),
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Every X Combo 2"),
                                    CP_SDK::XUI::XUIDropdown::Make(),
                                    CP_SDK::XUI::XUIDropdown::Make(),
                                    CP_SDK::XUI::XUIDropdown::Make()
                                }),
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Combo Break Above"),
                                    CP_SDK::XUI::XUIDropdown::Make(),
                                    CP_SDK::XUI::XUIDropdown::Make(),
                                    CP_SDK::XUI::XUIDropdown::Make()
                                })
                            })
                            ->SetSpacing(-0.3f)
                            ->AsShared()
                        }
                    })
                },*/
                {
                    u"Controller Expressions",
                    CP_SDK::XUI::XUITabControl::Make({
                        {
                            u"Buttons",
                            CP_SDK::XUI::XUIVLayout::Make({
                                //Y
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Y (Left controller top)"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&YTriggerDropdown)
                                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                                        ->OnValueChanged([](int, const std::u16string_view val)
                                        {
                                            auto settings = Config::ConfigManager::GetControllerTriggerSettings();
                                            settings.Y = to_utf8(val);
                                            Config::ConfigManager::SetControllerTriggerSettings(settings);
                                        })
                                        ->AsShared()
                                }),
                                //X
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"X (Left controller bottom)"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&XTriggerDropdown)
                                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                                        ->OnValueChanged([](int, const std::u16string_view val)
                                        {
                                            auto settings = Config::ConfigManager::GetControllerTriggerSettings();
                                            settings.X = to_utf8(val);
                                            Config::ConfigManager::SetControllerTriggerSettings(settings);
                                        })
                                        ->AsShared()
                                }),
                                //B
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"B (Right controller top)"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&BTriggerDropdown)
                                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                                        ->OnValueChanged([](int, const std::u16string_view val)
                                        {
                                            auto settings = Config::ConfigManager::GetControllerTriggerSettings();
                                            settings.B = to_utf8(val);
                                            Config::ConfigManager::SetControllerTriggerSettings(settings);
                                        })
                                        ->AsShared()
                                }),
                                //A
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"A (Right controller bottom)"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&ATriggerDropdown)
                                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                                        ->OnValueChanged([](int, const std::u16string_view val)
                                        {
                                            auto settings = Config::ConfigManager::GetControllerTriggerSettings();
                                            settings.A = to_utf8(val);
                                            Config::ConfigManager::SetControllerTriggerSettings(settings);
                                        })
                                        ->AsShared()
                                })
                            })
                        },
                        {
                            u"Triggers",
                            CP_SDK::XUI::XUIVLayout::Make({
                                //LGrip
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Left Grip"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&LGripTriggerDropdown)
                                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                                        ->OnValueChanged([](int, const std::u16string_view val)
                                        {
                                            auto settings = Config::ConfigManager::GetControllerTriggerSettings();
                                            settings.LGrip = to_utf8(val);
                                            Config::ConfigManager::SetControllerTriggerSettings(settings);
                                        })
                                        ->AsShared()
                                }),
                                //RGrip
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Right Grip"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&RGripTriggerDropdown)
                                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                                        ->OnValueChanged([](int, const std::u16string_view val)
                                        {
                                            auto settings = Config::ConfigManager::GetControllerTriggerSettings();
                                            settings.RGrip = to_utf8(val);
                                            Config::ConfigManager::SetControllerTriggerSettings(settings);
                                        })
                                        ->AsShared()
                                }),
                                //LTrigger
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Left Trigger"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&LTriggerTriggerDropdown)
                                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                                        ->OnValueChanged([](int, const std::u16string_view val)
                                        {
                                            auto settings = Config::ConfigManager::GetControllerTriggerSettings();
                                            settings.LTrigger = to_utf8(val);
                                            Config::ConfigManager::SetControllerTriggerSettings(settings);
                                        })
                                        ->AsShared()
                                }),
                                //RTrigger
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Right Trigger"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&RTriggerTriggerDropdown)
                                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                                        ->OnValueChanged([](int, const std::u16string_view val)
                                        {
                                            auto settings = Config::ConfigManager::GetControllerTriggerSettings();
                                            settings.RTrigger = to_utf8(val);
                                            Config::ConfigManager::SetControllerTriggerSettings(settings);
                                        })
                                        ->AsShared()
                                })
                            })
                        }
                    })
                },
                /*{
                    u"Lip Sync",
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIHLayout::Make({
                            CP_SDK::XUI::XUIText::Make(u"Enabled"),
                            CP_SDK::XUI::XUIToggle::Make(),
                        }),
                        CP_SDK::XUI::XUIHLayout::Make({
                            CP_SDK::XUI::XUIHLayout::Make({
                                CP_SDK::XUI::XUIVLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Gain"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                    CP_SDK::XUI::XUIText::Make(u"Threshold"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                })
                                ->SetSpacing(-0.3f)
                                ->AsShared(),
                                CP_SDK::XUI::XUIVLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Weight"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                    CP_SDK::XUI::XUIText::Make(u"Framerate"),
                                    CP_SDK::XUI::XUISlider::Make()
                                })
                                ->SetSpacing(-0.3f)
                                ->AsShared()
                            }),
                            CP_SDK::XUI::XUIHLayout::Make({
                                CP_SDK::XUI::XUIVLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Weight aa"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                    CP_SDK::XUI::XUIText::Make(u"Weight E"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                })
                                ->SetSpacing(-0.3f)
                                ->AsShared(),
                                CP_SDK::XUI::XUIVLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Weight ih"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                    CP_SDK::XUI::XUIText::Make(u"Weight oh"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                    CP_SDK::XUI::XUIText::Make(u"Weight ou"),
                                    CP_SDK::XUI::XUISlider::Make()
                                })
                                ->SetSpacing(-0.3f)
                                ->AsShared()
                            })
                        })
                    })
                    ->SetSpacing(-5.0f)
                    ->AsShared()
                }*/
            })
            ->OnActiveChanged([this](const int tab)
            {
                const std::vector opts = { "BlendShapes", "Controller Expressions" };
                faceTab = opts[tab];
                selectedTab = faceTab;
                TabSelected();
            })
            ->AsShared();
    }

    void AvatarSettingsViewController::UpdateControllerTriggerTab()
    {
        std::vector<std::u16string> options = { u"None" };
        if(AvatarManager::currentContext != nullptr)
        {
            const auto blendShapeController = AvatarManager::currentContext->rootGameObject->GetComponent<BlendShape::BlendShapeController*>();
            for (auto const& [key, val] : blendShapeController->blendShapeMappings)
            {
                options.push_back(to_utf16(val.name));
            }
        }
        const auto config = Config::ConfigManager::GetControllerTriggerSettings();

        ATriggerDropdown->SetOptions(options);
        ATriggerDropdown->SetValue(to_utf16(config.A));
        ATriggerDropdown->SetInteractable(AvatarManager::currentContext != nullptr);

        BTriggerDropdown->SetOptions(options);
        BTriggerDropdown->SetValue(to_utf16(config.B));
        BTriggerDropdown->SetInteractable(AvatarManager::currentContext != nullptr);

        XTriggerDropdown->SetOptions(options);
        XTriggerDropdown->SetValue(to_utf16(config.X));
        XTriggerDropdown->SetInteractable(AvatarManager::currentContext != nullptr);

        YTriggerDropdown->SetOptions(options);
        YTriggerDropdown->SetValue(to_utf16(config.Y));
        YTriggerDropdown->SetInteractable(AvatarManager::currentContext != nullptr);

        LGripTriggerDropdown->SetOptions(options);
        LGripTriggerDropdown->SetValue(to_utf16(config.LGrip));
        LGripTriggerDropdown->SetInteractable(AvatarManager::currentContext != nullptr);

        RGripTriggerDropdown->SetOptions(options);
        RGripTriggerDropdown->SetValue(to_utf16(config.RGrip));
        RGripTriggerDropdown->SetInteractable(AvatarManager::currentContext != nullptr);

        LTriggerTriggerDropdown->SetOptions(options);
        LTriggerTriggerDropdown->SetValue(to_utf16(config.LTrigger));
        LTriggerTriggerDropdown->SetInteractable(AvatarManager::currentContext != nullptr);

        RTriggerTriggerDropdown->SetOptions(options);
        RTriggerTriggerDropdown->SetValue(to_utf16(config.RTrigger));
        RTriggerTriggerDropdown->SetInteractable(AvatarManager::currentContext != nullptr);

    }

    void AvatarSettingsViewController::UpdateBlendshapesTab()
    {
        std::vector<std::u16string> options = { };
        if(AvatarManager::currentContext != nullptr)
        {
            const auto blendShapeController = AvatarManager::currentContext->rootGameObject->GetComponent<BlendShape::BlendShapeController*>();
            for (auto const& [key, val] : blendShapeController->blendShapeMappings)
            {
                options.push_back(to_utf16(val.name));
            }
        }

        const auto config = Config::ConfigManager::GetBlendshapeSettings();
        autoBlinkToggle->SetValue(config.autoBlink);
        autoBlinkWaitSlider->SetValue(config.waitTime);
        mockEyeMovementToggle->SetValue(config.mockEyeMovement);

        neutralExpressionDropdown->SetOptions(options);
        neutralExpressionDropdown->SetValue(to_utf16(config.neutralExpression));
        neutralExpressionDropdown->SetInteractable(AvatarManager::currentContext != nullptr);
    }

    std::shared_ptr<CP_SDK::XUI::XUITabControl> AvatarSettingsViewController::BuildVMCTab()
    {
        static std::vector<std::u16string> trackingOpts = { u"None", u"Waist", u"Left Foot", u"Right Foot", u"Left Knee", u"Right Knee"};
        return CP_SDK::XUI::XUITabControl::Make({
                {
                    u"Receiver",
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIText::Make(u"If using SlimeVR for FBT,\n Sender must be enabled")
                            ->SetFontSize(5)
                            ->AsShared(),
                        CP_SDK::XUI::XUIText::Make(u"VMC Receiver Enabled"),
                        CP_SDK::XUI::XUIToggle::Make()
                            ->SetValue(Config::ConfigManager::GetVMCSettings().enableReceiver)
                            ->OnValueChanged([](const bool val)
                            {
                                auto settings = Config::ConfigManager::GetVMCSettings();
                                settings.enableReceiver = val;
                                Config::ConfigManager::SetVMCSettings(settings);
                                VMC::VMCServer::InitServer();
                            })
                            ->AsShared(),

                        CP_SDK::XUI::XUIText::Make(u"VMC Receiver Port"),
                        CP_SDK::XUI::XUITextInput::Make(u"39539")
                            ->SetValue(to_utf16(Config::ConfigManager::GetVMCSettings().recvPort))
                            ->OnValueChanged([](const std::u16string_view str)
                            {
                                auto settings = Config::ConfigManager::GetVMCSettings();
                                settings.recvPort = to_utf8(str);
                                Config::ConfigManager::SetVMCSettings(settings);
                                VMC::VMCServer::InitServer();
                            })
                            ->AsShared()
                    })
                },
                {
                    u"Sender",
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIText::Make(u"VMC Sender Enabled"),
                        CP_SDK::XUI::XUIToggle::Make()
                            ->SetValue(Config::ConfigManager::GetVMCSettings().enableSender)
                            ->OnValueChanged([](const bool val)
                            {
                                auto settings = Config::ConfigManager::GetVMCSettings();
                                settings.enableSender = val;
                                Config::ConfigManager::SetVMCSettings(settings);
                                VMC::VMCClient::InitClient();
                            })
                            ->AsShared(),

                        CP_SDK::XUI::XUIText::Make(u"VMC Destination IP Adress"),
                        CP_SDK::XUI::XUITextInput::Make(u"127.0.0.1")
                            ->SetValue(to_utf16(Config::ConfigManager::GetVMCSettings().destIP))
                            ->OnValueChanged([](const std::u16string_view str)
                            {
                                auto settings = Config::ConfigManager::GetVMCSettings();
                                settings.destIP = to_utf8(str);
                                Config::ConfigManager::SetVMCSettings(settings);
                                VMC::VMCClient::InitClient();
                            })
                            ->AsShared(),

                        CP_SDK::XUI::XUIText::Make(u"VMC Destination Port"),
                        CP_SDK::XUI::XUITextInput::Make(u"39540")
                            ->SetValue(to_utf16(Config::ConfigManager::GetVMCSettings().destPort))
                            ->OnValueChanged([](const std::u16string_view str)
                            {
                                auto settings = Config::ConfigManager::GetVMCSettings();
                                settings.destPort = to_utf8(str);
                                Config::ConfigManager::SetVMCSettings(settings);
                                VMC::VMCClient::InitClient();
                            })
                            ->AsShared()
                    })
                },
                {
                    u"Full Body Tracking",
                    CP_SDK::XUI::XUIVLayout::Make({
                        CP_SDK::XUI::XUIText::Make(u"Requires the receiever to be enabled along with the sender if using SlimeVR")
                            ->SetFontSize(3)
                            ->AsShared(),
                        CP_SDK::XUI::XUIHLayout::Make(
                        {
                            CP_SDK::XUI::XUIText::Make(u"Enabled"),
                            CP_SDK::XUI::XUIToggle::Make()
                                ->SetValue(Config::ConfigManager::GetVMCSettings().enableFBT)
                                ->OnValueChanged([](const bool val)
                                {
                                    auto settings = Config::ConfigManager::GetVMCSettings();
                                    settings.enableFBT = val;
                                    Config::ConfigManager::SetVMCSettings(settings);
                                    AvatarManager::UpdateVRIK();
                                })
                                ->AsShared()
                        }),
                        CP_SDK::XUI::XUIHLayout::Make(
                        {
                            CP_SDK::XUI::XUIText::Make(u"Waist"),
                            CP_SDK::XUI::XUIDropdown::Make(trackingOpts)
                                ->OnValueChanged([](int idx, const std::u16string_view val)
                                {
                                    auto settings = Config::ConfigManager::GetVMCSettings();
                                    settings.waistTracker = to_utf8(val);
                                    Config::ConfigManager::SetVMCSettings(settings);
                                    AvatarManager::UpdateVRIK();
                                })
                                ->OnReady([](CP_SDK::UI::Components::CDropdown* dropdown)
                                {
                                    dropdown->SetValue(to_utf16(Config::ConfigManager::GetVMCSettings().waistTracker));
                                })
                                ->AsShared(),
                            CP_SDK::XUI::XUIText::Make(u"Chest"),
                            CP_SDK::XUI::XUIDropdown::Make(trackingOpts)
                                ->OnValueChanged([](int idx, const std::u16string_view val)
                                {
                                    auto settings = Config::ConfigManager::GetVMCSettings();
                                    settings.chestTracker = to_utf8(val);
                                    Config::ConfigManager::SetVMCSettings(settings);
                                    AvatarManager::UpdateVRIK();
                                })
                                ->OnReady([](CP_SDK::UI::Components::CDropdown* dropdown)
                                {
                                    dropdown->SetValue(to_utf16(Config::ConfigManager::GetVMCSettings().chestTracker));
                                })
                                ->AsShared()
                        }),
                        CP_SDK::XUI::XUIHLayout::Make(
                        {
                            CP_SDK::XUI::XUIText::Make(u"Left Foot"),
                            CP_SDK::XUI::XUIDropdown::Make(trackingOpts)
                                ->OnValueChanged([](int idx, const std::u16string_view val)
                                {
                                    auto settings = Config::ConfigManager::GetVMCSettings();
                                    settings.leftFoot = to_utf8(val);
                                    Config::ConfigManager::SetVMCSettings(settings);
                                    AvatarManager::UpdateVRIK();
                                })
                                ->OnReady([](CP_SDK::UI::Components::CDropdown* dropdown)
                                {
                                    dropdown->SetValue(to_utf16(Config::ConfigManager::GetVMCSettings().leftFoot));
                                })
                                ->AsShared(),
                            CP_SDK::XUI::XUIText::Make(u"Left Knee"),
                            CP_SDK::XUI::XUIDropdown::Make(trackingOpts)
                                ->OnValueChanged([](int idx, const std::u16string_view val)
                                {
                                    auto settings = Config::ConfigManager::GetVMCSettings();
                                    settings.leftKnee = to_utf8(val);
                                    Config::ConfigManager::SetVMCSettings(settings);
                                    AvatarManager::UpdateVRIK();
                                })
                                ->OnReady([](CP_SDK::UI::Components::CDropdown* dropdown)
                                {
                                    dropdown->SetValue(to_utf16(Config::ConfigManager::GetVMCSettings().leftKnee));
                                })
                                ->AsShared()
                        }),
                        CP_SDK::XUI::XUIHLayout::Make(
                        {
                            CP_SDK::XUI::XUIText::Make(u"Right Foot"),
                            CP_SDK::XUI::XUIDropdown::Make(trackingOpts)
                                ->OnValueChanged([](int idx, const std::u16string_view val)
                                {
                                    auto settings = Config::ConfigManager::GetVMCSettings();
                                    settings.rightFoot = to_utf8(val);
                                    Config::ConfigManager::SetVMCSettings(settings);
                                    AvatarManager::UpdateVRIK();
                                })
                                ->OnReady([](CP_SDK::UI::Components::CDropdown* dropdown)
                                {
                                    dropdown->SetValue(to_utf16(Config::ConfigManager::GetVMCSettings().rightFoot));
                                })
                                ->AsShared(),
                            CP_SDK::XUI::XUIText::Make(u"Right Knee"),
                            CP_SDK::XUI::XUIDropdown::Make(trackingOpts)
                                ->OnValueChanged([](int idx, const std::u16string_view val)
                                {
                                    auto settings = Config::ConfigManager::GetVMCSettings();
                                    settings.rightKnee = to_utf8(val);
                                    Config::ConfigManager::SetVMCSettings(settings);
                                    AvatarManager::UpdateVRIK();
                                })
                                ->OnReady([](CP_SDK::UI::Components::CDropdown* dropdown)
                                {
                                    dropdown->SetValue(to_utf16(Config::ConfigManager::GetVMCSettings().rightKnee));
                                })
                                ->AsShared()
                        }),
                    })
                    ->OnReady([](CP_SDK::UI::Components::CVLayout* layout)
                    {
                        layout->SetSpacing(-0.7f);
                    })
                    ->AsShared()
                }
            });
    }

    std::shared_ptr<CP_SDK::XUI::XUIHLayout> AvatarSettingsViewController::BuildIKSettingsTab()
    {
        return CP_SDK::XUI::XUIHLayout::Make(
            {
                CP_SDK::XUI::XUIVLayout::Make(
                {
                    CP_SDK::XUI::XUIText::Make(u"Leg Swivel"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(100.0f)
                        ->SetMinValue(-50.0f)
                        ->SetMaxValue(50.0f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().legSwivel)
                        ->Bind(&legSwivelSlider)
                        ->OnValueChanged([](const float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.legSwivel = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Arm Swivel"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(100.0f)
                        ->SetMinValue(-50.0f)
                        ->SetMaxValue(50.0f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().armSwivel)
                        ->Bind(&armSwivelSlider)
                        ->OnValueChanged([](const float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.armSwivel = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Body Stiffness"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(100.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(5.0f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().bodyStiffness)
                        ->Bind(&bodyStiffnessSlider)
                        ->OnValueChanged([](const float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.bodyStiffness = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                    ->AsShared(),
                }),
                CP_SDK::XUI::XUIVLayout::Make(
                {
                    CP_SDK::XUI::XUIText::Make(u"Shoulder Rotation Weight"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(10.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(1.0f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().shoulderRotationWeight)
                        ->Bind(&shoulderRotationWeightSlider)
                        ->OnValueChanged([](const float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.shoulderRotationWeight = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Ground Y Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(100.0f)
                        ->SetMinValue(-2.0f)
                        ->SetMaxValue(2.0f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().groundOffset)
                        ->Bind(&groundOffsetSlider)
                        ->OnValueChanged([](const float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.groundOffset = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                        ->AsShared()
                    /*CP_SDK::XUI::XUIText::Make(u"Wrist Twist Fix Amount"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(80.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(0.8f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().wristTwistFixAmount)
                        ->Bind(&wristTwistFixSlider)
                        ->OnValueChanged([](const float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.wristTwistFixAmount = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Shoulder Twist Fix Amount"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(80.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(0.8f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().shoulderTwistFixAmount)
                        ->Bind(&shoulderTwistFixSlider)
                        ->OnValueChanged([](const float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.shoulderTwistFixAmount = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                        ->AsShared()*/
                }),
            }
        );
    }

    void AvatarSettingsViewController::UpdateIKTab()
    {
        legSwivelSlider->SetValue(Config::ConfigManager::GetIKSettings().legSwivel);
        armSwivelSlider->SetValue(Config::ConfigManager::GetIKSettings().armSwivel);
        bodyStiffnessSlider->SetValue(Config::ConfigManager::GetIKSettings().bodyStiffness);
        groundOffsetSlider->SetValue(Config::ConfigManager::GetIKSettings().groundOffset);
        shoulderRotationWeightSlider->SetValue(Config::ConfigManager::GetIKSettings().shoulderRotationWeight);
        //wristTwistFixSlider->SetValue(Config::ConfigManager::GetIKSettings().wristTwistFixAmount);
        //shoulderTwistFixSlider->SetValue(Config::ConfigManager::GetIKSettings().shoulderTwistFixAmount);
    }

    std::shared_ptr<CP_SDK::XUI::XUISlider> AvatarSettingsViewController::BuildFingerSlider(const int finger)
    {
        int idx = finger;
        auto slider = CP_SDK::XUI::XUISlider::Make()
            ->SetIncrements(240.0f)
            ->SetMinValue(-120)
            ->SetMaxValue(120)
            ->SetInteger(true)
            ->SetValue(HandController::GetValue(Config::ConfigManager::GetFingerPoseSettings().grabPose, idx))
            ->OnValueChanged([idx](const float val)
            {
                auto settings = Config::ConfigManager::GetFingerPoseSettings();
                settings.grabPose = HandController::SetValue(settings.grabPose, idx, val);
                Config::ConfigManager::SetFingerPoseSettings(settings);
                AvatarManager::SetFingerPose(Config::ConfigManager::GetFingerPoseSettings().grabPose);
            })
            ->AsShared();
        fingerSliders[idx] = slider;
        return slider;
    }

    // very longggg
    std::shared_ptr<CP_SDK::XUI::XUITabControl> AvatarSettingsViewController::BuildFingerPoseSettingsTab()
    {
        return CP_SDK::XUI::XUITabControl::Make({
                    {
                        u"???",
                        CP_SDK::XUI::XUIHLayout::Make({
                            CP_SDK::XUI::XUIImage::Make()
                                ->SetSprite(BSML::Utilities::LoadSpriteRaw(IncludedAssets::human_hand_png))
                                ->SetWidth(45)
                                ->SetHeight(45)
                                ->AsShared(),
                            CP_SDK::XUI::XUIText::Make(u"Horizontal Angle can help make fingers \n more spread out or closer together")
                                ->SetFontSize(3.5)
                                ->AsShared()
                        })
                        ->OnReady([](CP_SDK::UI::Components::CHLayout* layout) -> void
                        {
                            layout->HOrVLayoutGroup()->set_childAlignment(UnityEngine::TextAnchor::MiddleCenter);
                        })
                        ->SetSpacing(1)
                        ->AsShared()
                    },
                    {
                        u"Little",
                        CP_SDK::XUI::XUIHLayout::Make({
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Little Distal"),
                                BuildFingerSlider(0),

                                CP_SDK::XUI::XUIText::Make(u"Little Intermediate"),
                                BuildFingerSlider(1),

                                CP_SDK::XUI::XUIText::Make(u"Little Proximal"),
                                BuildFingerSlider(2)
                            })
                            ->SetSpacing(-0.02f)
                            ->AsShared(),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Little Proximal Horizontal Angle"),
                                BuildFingerSlider(3)
                            })
                            ->SetSpacing(-0.02f)
                            ->AsShared()
                        })
                    },
                    {
                        u"Ring",
                        CP_SDK::XUI::XUIHLayout::Make({
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Ring Distal"),
                                BuildFingerSlider(4),

                                CP_SDK::XUI::XUIText::Make(u"Ring Intermediate"),
                                BuildFingerSlider(5),

                                CP_SDK::XUI::XUIText::Make(u"Ring Proximal"),
                                BuildFingerSlider(6)
                            })
                            ->SetSpacing(-0.02f)
                            ->AsShared(),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Ring Proximal Horizontal Angle"),
                                BuildFingerSlider(7)
                            })
                            ->SetSpacing(-0.02f)
                            ->AsShared()
                        })
                    },
                    {
                        u"Middle",
                        CP_SDK::XUI::XUIHLayout::Make({
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Middle Distal"),
                                BuildFingerSlider(8),

                                CP_SDK::XUI::XUIText::Make(u"Middle Intermediate"),
                                BuildFingerSlider(9),

                                CP_SDK::XUI::XUIText::Make(u"Middle Proximal"),
                                BuildFingerSlider(10)
                            })
                            ->SetSpacing(-0.02f)
                            ->AsShared(),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Middle Proximal Horizontal Angle"),
                                BuildFingerSlider(11)
                            })
                            ->SetSpacing(-0.02f)
                            ->AsShared()
                        })
                    },
                    {
                        u"Index",
                        CP_SDK::XUI::XUIHLayout::Make({
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Index Distal"),
                                BuildFingerSlider(12),

                                CP_SDK::XUI::XUIText::Make(u"Index Intermediate"),
                                BuildFingerSlider(13),

                                CP_SDK::XUI::XUIText::Make(u"Index Proximal"),
                                 BuildFingerSlider(14)
                            })
                            ->SetSpacing(-0.02f)
                            ->AsShared(),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Index Proximal Horizontal Angle"),
                                BuildFingerSlider(15)
                            })
                            ->SetSpacing(-0.02f)
                            ->AsShared()
                        })
                    },
                    {
                        u"Thumb",
                        CP_SDK::XUI::XUIHLayout::Make({
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Thumb Distal"),
                                BuildFingerSlider(16),

                                CP_SDK::XUI::XUIText::Make(u"Thumb Intermediate"),
                                BuildFingerSlider(17),

                                CP_SDK::XUI::XUIText::Make(u"Thumb Proximal"),
                                BuildFingerSlider(18)
                            })
                            ->SetSpacing(-0.02f)
                            ->AsShared(),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Thumb Proximal Horizontal Angle"),
                                BuildFingerSlider(19)
                            })
                            ->SetSpacing(-0.02f)
                            ->AsShared()
                        })
                    }
                });
    }

    void AvatarSettingsViewController::UpdateFingerPosingTab()
    {
        for (int i = 0; i < fingerSliders.size(); ++i)
        {
            const auto slider = fingerSliders[i];
            slider->SetValue(HandController::GetValue(Config::ConfigManager::GetFingerPoseSettings().grabPose, i));
        }
    }

    std::shared_ptr<CP_SDK::XUI::XUIHLayout> AvatarSettingsViewController::BuildLocoSettingsTab()
    {
        return CP_SDK::XUI::XUIHLayout::Make({
            CP_SDK::XUI::XUIVLayout::Make({
                CP_SDK::XUI::XUIHLayout::Make({
                    CP_SDK::XUI::XUIText::Make(u"Foot Distance"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(30.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(0.3f)
                        ->SetValue(Config::ConfigManager::GetLocomotionSettings().footDistance)
                        ->Bind(&footDistanceSlider)
                        ->OnValueChanged([](const float val)
                        {
                            auto settings = Config::ConfigManager::GetLocomotionSettings();
                            settings.footDistance = val;
                            Config::ConfigManager::SetLocomotionSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Step Threshold"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(30.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(0.3f)
                        ->SetValue(Config::ConfigManager::GetLocomotionSettings().stepThreshold)
                        ->Bind(&stepThresholdSlider)
                        ->OnValueChanged([](const float val)
                        {
                            auto settings = Config::ConfigManager::GetLocomotionSettings();
                            settings.stepThreshold = val;
                            Config::ConfigManager::SetLocomotionSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                        ->AsShared(),
                }),
                CP_SDK::XUI::XUIHLayout::Make({
                    CP_SDK::XUI::XUIText::Make(u"Step Height"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(10.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(0.1f)
                        ->SetValue(Config::ConfigManager::GetLocomotionSettings().stepHeight)
                        ->Bind(&stepHeightSlider)
                        ->OnValueChanged([](const float val)
                        {
                            auto settings = Config::ConfigManager::GetLocomotionSettings();
                            settings.stepHeight = val;
                            Config::ConfigManager::SetLocomotionSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Step Offset Z"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(40.0f)
                        ->SetMinValue(-0.2f)
                        ->SetMaxValue(0.2f)
                        ->SetValue(Config::ConfigManager::GetLocomotionSettings().stepOffset.z)
                        ->Bind(&stepOffsetZSlider)
                        ->OnValueChanged([](const float val)
                        {
                            auto settings = Config::ConfigManager::GetLocomotionSettings();
                            settings.stepOffset.z = val;
                            Config::ConfigManager::SetLocomotionSettings(settings);
                            AvatarManager::UpdateVRIK();
                        })
                        ->AsShared()
                })
            })
        });
    }

    void AvatarSettingsViewController::UpdateLocomotionTab()
    {
        footDistanceSlider->SetValue(Config::ConfigManager::GetLocomotionSettings().footDistance);
        stepThresholdSlider->SetValue(Config::ConfigManager::GetLocomotionSettings().stepThreshold);
        stepOffsetZSlider->SetValue(Config::ConfigManager::GetLocomotionSettings().stepOffset.z);
        stepHeightSlider->SetValue(Config::ConfigManager::GetLocomotionSettings().stepHeight);
    }

    std::shared_ptr<CP_SDK::XUI::XUIVLayout> AvatarSettingsViewController::BuildMirrorTab()
    {
        const std::vector<std::u16string> layerOptions = {
            u"All",
            u"All Except UI",
            u"Only Avatar",
            u"Avatar and Saber",
            u"Avatar, Saber, and Notes",
            u"Avatar, Saber, Notes, and Walls",
        };

        const std::vector<std::u16string> trackOptions = {
            u"None",
            u"Waist",
            u"Face",
            u"Chest"
        };

        const std::vector<std::u16string> sceneOptions = {
            u"Both",
            u"Menu",
            u"Game"
        };

        return CP_SDK::XUI::XUIVLayout::Make({
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUIText::Make(u"Enabled"),
                CP_SDK::XUI::XUIToggle::Make()
                    ->SetValue(Config::ConfigManager::GetMirrorSettings().enabled)
                    ->OnValueChanged([](const bool val)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.enabled = val;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(false);
                    })
                    ->AsShared(),
                CP_SDK::XUI::XUIText::Make(u"Scene"),
                CP_SDK::XUI::XUIDropdown::Make()
                    ->SetOptions(sceneOptions)
                    ->OnValueChanged([](const int idx, std::u16string_view)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.scene = idx;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(false);
                    })
                    ->OnReady([sceneOptions](CP_SDK::UI::Components::CDropdown* dropdown)
                    {
                        dropdown->SetValue(sceneOptions[Config::ConfigManager::GetMirrorSettings().scene]);
                    })
                    ->AsShared()
            }),
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUIText::Make(u"Size"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(1.0f)
                    ->SetMaxValue(150.0f)
                    ->SetValue(Config::ConfigManager::GetMirrorSettings().size)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.size = val;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(true);
                    })
                    ->AsShared(),
                CP_SDK::XUI::XUIText::Make(u"Aspect"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(0.25f)
                    ->SetMaxValue(4.0f)
                    ->SetValue(Config::ConfigManager::GetMirrorSettings().aspect)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.aspect = val;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(true);
                    })
                    ->AsShared()
            }),
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUIText::Make(u"Field Of View"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(10.f)
                    ->SetMaxValue(120.0f)
                    ->SetValue(Config::ConfigManager::GetMirrorSettings().fov)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.fov = val;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(false);
                    })
                    ->AsShared(),
                CP_SDK::XUI::XUIText::Make(u"Near Clip"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(0.1f)
                    ->SetMaxValue(3.0f)
                    ->SetValue(Config::ConfigManager::GetMirrorSettings().nearClip)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.nearClip = val;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(false);
                    })
                    ->AsShared()
            }),
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUIText::Make(u"Displayed Layer"),
                CP_SDK::XUI::XUIDropdown::Make()
                    ->SetOptions(layerOptions)
                    ->OnValueChanged([](const int idx, std::u16string_view)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.layer = idx;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(false);
                    })
                    ->OnReady([layerOptions](CP_SDK::UI::Components::CDropdown* dropdown)
                    {
                        dropdown->SetValue(layerOptions[Config::ConfigManager::GetMirrorSettings().layer]);
                    })
                    ->AsShared(),
                CP_SDK::XUI::XUIText::Make(u"Transparent Background"),
                CP_SDK::XUI::XUIToggle::Make()
                    ->SetValue(Config::ConfigManager::GetMirrorSettings().transparentBackground)
                    ->OnValueChanged([](const bool val)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.transparentBackground = val;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(false);
                    })
                    ->AsShared()
            }),
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUIText::Make(u"Tracked Bone"),
                CP_SDK::XUI::XUIDropdown::Make()
                    ->SetOptions(trackOptions)
                    ->OnValueChanged([](const int idx, std::u16string_view)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.boneTracking = idx;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(false);
                    })
                    ->OnReady([trackOptions](CP_SDK::UI::Components::CDropdown* dropdown)
                    {
                        dropdown->SetValue(trackOptions[Config::ConfigManager::GetMirrorSettings().boneTracking]);
                    })
                    ->AsShared(),
                CP_SDK::XUI::XUIText::Make(u"Tracking Distance"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(0.5f)
                    ->SetMaxValue(15.0f)
                    ->SetValue(Config::ConfigManager::GetMirrorSettings().distance)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.distance = val;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(false);
                    })
                    ->AsShared(),
            }),
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUIText::Make(u"Show Handle"),
                CP_SDK::XUI::XUIToggle::Make()
                    ->SetValue(Config::ConfigManager::GetMirrorSettings().showHandle)
                    ->OnValueChanged([](const bool val)
                    {
                        auto settings = Config::ConfigManager::GetMirrorSettings();
                        settings.showHandle = val;
                        Config::ConfigManager::SetMirrorSettings(settings);
                        MirrorManager::UpdateMirror(false);
                    })
                    ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Border Color"),
                    CP_SDK::XUI::XUIColorInput::Make()
                        ->SetValue(Config::ConfigManager::GetMirrorSettings().borderColor)
                        ->SetAlphaSupport(true)
                        ->OnValueChanged([](const UnityEngine::Color val)
                        {
                            auto settings = Config::ConfigManager::GetMirrorSettings();
                            settings.borderColor = val;
                            Config::ConfigManager::SetMirrorSettings(settings);
                            MirrorManager::UpdateMirror(false);
                        })
                        ->AsShared()
            }),
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUIText::Make(u"<i>Transparency does not currently work with bloom enabled</i>")
            })
        })
        ->SetSpacing(-1.2f)
        ->AsShared();
    }

    std::shared_ptr<CP_SDK::XUI::XUIHLayout> AvatarSettingsViewController::BuildLightingTab()
    {
        return CP_SDK::XUI::XUIHLayout::Make(
        {
            CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"Global Light Color"),
                CP_SDK::XUI::XUIColorInput::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().globalColor)
                    ->Bind(&globalColorInput)
                    ->OnValueChanged([](const UnityEngine::Color val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.globalColor = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    })
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Light Intensity"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(2.5f)
                    ->SetValue(Config::ConfigManager::GetLightingSettings().globalLightIntensity)
                    ->Bind(&globalIntensitySlider)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.globalLightIntensity = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    })
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Light Rotation X"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetIncrements(360.0f)
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(360.0f)
                    ->SetValue(Config::ConfigManager::GetLightingSettings().lightRotation.x)
                    ->Bind(&lightRotationXSlider)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.lightRotation.x = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    })
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Light Rotation Y"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetIncrements(360.0f)
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(360.0f)
                    ->SetValue(Config::ConfigManager::GetLightingSettings().lightRotation.y)
                    ->Bind(&lightRotationYSlider)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.lightRotation.y = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    })
                    ->AsShared()
            }),

            CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"Beatmap Lighting"),
                CP_SDK::XUI::XUIToggle::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLighting)
                    ->Bind(&beatmapLightingToggle)
                    ->OnValueChanged([](const bool val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.beatmapLighting = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    })
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"BM Lighting Brightness"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLightingBrightness)
                    ->Bind(&beatmapLightingBrightnessSlider)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.beatmapLightingBrightness = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    })
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"BM Lighting Saturation"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLightingColorIntensity)
                    ->Bind(&beatmapLightingColorIntensitySlider)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.beatmapLightingColorIntensity = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    })
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"BM Lighting Min Brightness"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLightingMinimumBrightness)
                    ->Bind(&beatmapLightingMinBrightnessSlider)
                    ->OnValueChanged([](const float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.beatmapLightingMinimumBrightness = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    })
                    ->AsShared(),
            })
        });
    }

    void AvatarSettingsViewController::UpdateLightingTab()
    {
        globalColorInput->SetValue(Config::ConfigManager::GetLightingSettings().globalColor);
        globalIntensitySlider->SetValue(Config::ConfigManager::GetLightingSettings().globalLightIntensity);
        lightRotationXSlider->SetValue(Config::ConfigManager::GetLightingSettings().lightRotation.x);
        lightRotationYSlider->SetValue(Config::ConfigManager::GetLightingSettings().lightRotation.y);
        beatmapLightingToggle->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLighting);
        beatmapLightingBrightnessSlider->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLightingBrightness);
        beatmapLightingMinBrightnessSlider->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLightingMinimumBrightness);
    }

    std::shared_ptr<CP_SDK::XUI::XUIVLayout> AvatarSettingsViewController::BuildWindTab()
    {
        return CP_SDK::XUI::XUIVLayout::Make({
            CP_SDK::XUI::XUIText::Make(u"Enable Wind"),
            CP_SDK::XUI::XUIToggle::Make()
                ->Bind(&enableWindToggle)
                ->SetValue(Config::ConfigManager::GetWindSettings().enabled)
                ->OnValueChanged([](const bool val)
                {
                    auto settings = Config::ConfigManager::GetWindSettings();
                    settings.enabled = val;
                    Config::ConfigManager::SetWindSettings(settings);
                    if(AvatarManager::currentContext != nullptr)
                    {
                        const auto wind = AvatarManager::currentContext->rootGameObject->GetComponent<VRMWind*>();
                        wind->enableWind = val;
                        if(!val)
                        {
                            wind->DisableWind();
                        }
                    }
                })
                ->AsShared(),
            CP_SDK::XUI::XUIText::Make(u"Strength Force"),
            CP_SDK::XUI::XUISlider::Make()
                ->Bind(&strengthSlider)
                ->SetMinValue(0.1f)
                ->SetMaxValue(7.5f)
                ->SetValue(Config::ConfigManager::GetWindSettings().windStrength)
                ->OnValueChanged([](const float val)
                {
                    auto settings = Config::ConfigManager::GetWindSettings();
                    settings.windStrength = val;
                    Config::ConfigManager::SetWindSettings(settings);
                    if(AvatarManager::currentContext != nullptr)
                    {
                        const auto wind = AvatarManager::currentContext->rootGameObject->GetComponent<VRMWind*>();
                        wind->strengthFactor = val;
                    }
                })
                ->AsShared(),
            CP_SDK::XUI::XUIText::Make(u"Time Factor"),
            CP_SDK::XUI::XUISlider::Make()
                ->Bind(&timeForceSlider)
                ->SetMinValue(0.1f)
                ->SetMaxValue(7.5f)
                ->SetValue(Config::ConfigManager::GetWindSettings().timeFactor)
                ->OnValueChanged([](const float val)
                {
                    auto settings = Config::ConfigManager::GetWindSettings();
                    settings.timeFactor = val;
                    Config::ConfigManager::SetWindSettings(settings);
                    if(AvatarManager::currentContext != nullptr)
                    {
                        const auto wind = AvatarManager::currentContext->rootGameObject->GetComponent<VRMWind*>();
                        wind->timeFactor = val;
                    }
                })
                ->AsShared(),
        });
    }

    void AvatarSettingsViewController::UpdateWindTab()
    {
        const auto settings = Config::ConfigManager::GetWindSettings();
        enableWindToggle->SetValue(settings.enabled);
        strengthSlider->SetValue(settings.windStrength);
        timeForceSlider->SetValue(settings.timeFactor);
    }
}
