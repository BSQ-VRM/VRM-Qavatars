#include "UI/AvatarSettingsViewController.hpp"

#include <HandController.hpp>
#include <sstream>

#include "LightManager.hpp"

#include "assets.hpp"
#include "AvatarManager.hpp"
#include "customTypes/TargetManager.hpp"

#include <string_view>
#include <questui/shared/BeatSaberUI.hpp>
#include <questui/shared/CustomTypes/Components/List/QuestUIBoxTableCell.hpp>

#include "chatplex-sdk-bs/shared/CP_SDK/XUI/Templates.hpp"
#include "config/ConfigManager.hpp"
#include "customTypes/BlendShape/BlendShapeController.hpp"

namespace VRMQavatars::UI::ViewControllers {
    CP_SDK_IL2CPP_INHERIT_INIT(AvatarSettingsViewController);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(AvatarSettingsViewController)
    {
        OnViewCreation      = {this, &AvatarSettingsViewController::DidActivate};
    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(AvatarSettingsViewController)
    {

    }

    void AvatarSettingsViewController::DidActivate()
    {
        configModal = CreateModal<Modals::IndividualConfigModal>();
        CP_SDK::XUI::Templates::FullRectLayoutMainView({
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUIIconButton::Make(QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::settings_png))
                    ->OnClick(CP_SDK::Utils::Action<>([this]
                    {
                        ShowModal(configModal.Ptr());
                    }))
                    ->SetWidth(6.0f)
                    ->SetHeight(6.0f)
                    ->AsShared(),
                CP_SDK::XUI::Templates::TitleBar(u"Avatar Settings")->AsShared(),
            }),
            CP_SDK::XUI::XUITabControl::Make(u"Settings Tab", {
                { u"Calibration", BuildCalibrationTab() },
                { u"Hands", BuildHandOffsetsTab() },
                { u"Face", BuildFaceTab() },
                { u"VMC", BuildVMCTab() },
                { u"Fingers", BuildFingerPoseSettingsTab() },
                { u"IK", BuildIKSettingsTab() },
                { u"Locomotion", BuildLocoSettingsTab() },
                { u"Lighting", BuildLightingTab() },
            })
        })
        ->SetSpacing(1.0f)
        ->BuildUI(get_transform());

        AvatarManager::OnLoad += CP_SDK::Utils::Action<>([this]
        {
            UpdateIKTab();
            UpdateHandOffsetsTab();
            UpdateFingerPosingTab();
            UpdateControllerTriggerTab();
            UpdateLocomotionTab();
            UpdateLightingTab();
        });
    }

    void AvatarSettingsViewController::UpdatePos()
    {
        AvatarManager::SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
    }

    int GetValue(const std::string& pose, const int idx)
    {
        const auto values = HandController::ParseRotations(HandController::SplitPose(pose));
        if(values.size()-1 < idx) return 0;
        return values[idx];
    }

    std::string SetValue(const std::string& pose, const int idx, const int value)
    {
        auto values = HandController::ParseRotations(HandController::SplitPose(pose));
        values[idx] = value;

        auto newValues = std::vector<std::string>(values.size());
        for (int i = 0; i < values.size(); ++i)
        {
            newValues[i] = std::to_string(values[i]);
        }

        const char* const delim = ",";

        std::ostringstream imploded;
        std::copy(newValues.begin(), newValues.end(),
                   std::ostream_iterator<std::string>(imploded, delim));

        return imploded.str();
    }

    std::shared_ptr<CP_SDK::XUI::XUIVLayout> AvatarSettingsViewController::BuildCalibrationTab()
    {
        return CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"(Global)")
                    ->SetFontSize(6.0f)
                    ->AsShared(),
                CP_SDK::XUI::XUIVSpacer::Make(0.3f),

                CP_SDK::XUI::XUIText::Make(u"Calibration Type"),
                CP_SDK::XUI::XUIDropdown::Make({ u"Match Armspans", u"Match Heights", u"Fixed" })
                    ->SetValue(to_utf16(Config::ConfigManager::GetGlobalConfig().CalibrationType.GetValue()))
                    ->OnValueChanged(CP_SDK::Utils::Action<int, std::u16string_view>([this](int idx, std::u16string_view val)
                    {
                        fixedSlider->SetInteractable(val == u"Fixed");
                        Config::ConfigManager::GetGlobalConfig().CalibrationType.SetValue(to_utf8(val));
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIVSpacer::Make(0.2f),

                CP_SDK::XUI::XUIText::Make(u"Fixed Scale"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(0.2f)
                    ->SetMaxValue(2.0f)
                    ->SetValue(Config::ConfigManager::GetGlobalConfig().FixedScale.GetValue())
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        Config::ConfigManager::GetGlobalConfig().FixedScale.SetValue(val);
                    }))
                    ->SetIncrements(0.1f)
                    ->SetInteractable(Config::ConfigManager::GetGlobalConfig().CalibrationType.GetValue() == "Fixed")
                    ->Bind(&fixedSlider)
                    ->AsShared()
            }
        );
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
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.posX = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            UpdatePos();
                        }))
                        ->Bind(&xPosSlider)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Y Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 1.0f, 0.0f, 1.0f})
                        ->SetIncrements(20.0f)
                        ->SetMinValue(-0.5f)
                        ->SetMaxValue(0.5f)
                        ->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.posY)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.posY = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            UpdatePos();
                        }))
                        ->Bind(&yPosSlider)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Z Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 0.0f, 1.0f, 1.0f})
                        ->SetIncrements(20.0f)
                        ->SetMinValue(-0.5f)
                        ->SetMaxValue(0.5f)
                        ->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.posZ)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.posZ = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            UpdatePos();
                        }))
                        ->Bind(&zPosSlider)
                        ->AsShared(),
                }),

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
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.rotX = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            UpdatePos();
                        }))
                        ->Bind(&xRotSlider)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Y Rotation"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 1.0f, 0.0f, 1.0f})
                        ->SetIncrements(180.0f)
                        ->SetMinValue(-90.0f)
                        ->SetMaxValue(90.0f)
                        ->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.rotY)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.rotY = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            UpdatePos();
                        }))
                        ->Bind(&yRotSlider)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Z Rotation"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 0.0f, 1.0f, 1.0f})
                        ->SetIncrements(180.0f)
                        ->SetMinValue(-90.0f)
                        ->SetMaxValue(90.0f)
                        ->SetValue(Config::ConfigManager::GetOffsetSettings().handOffset.rotZ)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            auto settings = Config::ConfigManager::GetOffsetSettings();
                            settings.handOffset.rotZ = val;
                            Config::ConfigManager::SetOffsetSettings(settings);
                            UpdatePos();
                        }))
                        ->Bind(&zRotSlider)
                        ->AsShared()
                })
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
                                CP_SDK::XUI::XUIToggle::Make(),
                                CP_SDK::XUI::XUIText::Make(u"Blink Wait Time"),
                                CP_SDK::XUI::XUISlider::Make(),
                            })
                            ->SetSpacing(0.1f)
                            ->AsShared(),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Mock Eye Movement"),
                                CP_SDK::XUI::XUIToggle::Make(),
                                CP_SDK::XUI::XUIText::Make(u"Default Facial Expression"),
                                CP_SDK::XUI::XUIDropdown::Make()
                            })
                            ->SetSpacing(0.1f)
                            ->AsShared()
                        })
                    })
                },
                {
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
                                }),
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"On Combo Reached"),
                                    CP_SDK::XUI::XUIDropdown::Make(),
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
                        }
                    })
                },
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
                                        ->AsShared()
                                }),
                                //X
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"X (Left controller bottom)"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&XTriggerDropdown)
                                        ->AsShared()
                                }),
                                //B
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"B (Right controller top)"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&BTriggerDropdown)
                                        ->AsShared()
                                }),
                                //A
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"A (Right controller bottom)"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&ATriggerDropdown)
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
                                        ->AsShared()
                                }),
                                //RGrip
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Right Grip"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&RGripTriggerDropdown)
                                        ->AsShared()
                                }),
                                //LTrigger
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Left Trigger"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&LTriggerTriggerDropdown)
                                        ->AsShared()
                                }),
                                //RTrigger
                                CP_SDK::XUI::XUIHLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Right Trigger"),
                                    CP_SDK::XUI::XUIDropdown::Make({ u"None" })
                                        ->Bind(&RTriggerTriggerDropdown)
                                        ->AsShared()
                                })
                            })
                        }
                    })
                },
                {
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
                }
            });
    }

    void AvatarSettingsViewController::UpdateControllerTriggerTab()
    {
        std::vector<std::u16string> options = { u"None" };
        if(AvatarManager::currentContext != nullptr)
        {
            auto blendShapeController = AvatarManager::currentContext->rootGameObject->GetComponent<BlendShape::BlendShapeController*>();
            for (auto const& [key, val] : blendShapeController->blendShapeMappings)
            {
                options.push_back(to_utf16(val.name));
            }
        }
        auto config = Config::ConfigManager::GetControllerTriggerSettings();
        ATriggerDropdown->SetOptions(options);
        ATriggerDropdown->SetValue(to_utf16(config.A));
        BTriggerDropdown->SetOptions(options);
        BTriggerDropdown->SetValue(to_utf16(config.B));
        XTriggerDropdown->SetOptions(options);
        XTriggerDropdown->SetValue(to_utf16(config.X));
        YTriggerDropdown->SetOptions(options);
        YTriggerDropdown->SetValue(to_utf16(config.Y));
        LGripTriggerDropdown->SetOptions(options);
        LGripTriggerDropdown->SetValue(to_utf16(config.LGrip));
        RGripTriggerDropdown->SetOptions(options);
        RGripTriggerDropdown->SetValue(to_utf16(config.RGrip));
        LTriggerTriggerDropdown->SetOptions(options);
        LTriggerTriggerDropdown->SetValue(to_utf16(config.LTrigger));
        RTriggerTriggerDropdown->SetOptions(options);
        RTriggerTriggerDropdown->SetValue(to_utf16(config.RTrigger));
    }

    std::shared_ptr<CP_SDK::XUI::XUITabControl> AvatarSettingsViewController::BuildVMCTab()
    {
        return CP_SDK::XUI::XUITabControl::Make({
                {
                    u"Reveiver",
                    CP_SDK::XUI::XUIVLayout::Make({
                    })
                },
                {
                    u"Sender",
                    CP_SDK::XUI::XUIVLayout::Make({
                    })
                },
                {
                    u"FBT",
                    CP_SDK::XUI::XUIVLayout::Make({
                    })
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
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.legSwivel = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        }))
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Arm Swivel"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(100.0f)
                        ->SetMinValue(-50.0f)
                        ->SetMaxValue(50.0f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().armSwivel)
                        ->Bind(&armSwivelSlider)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.armSwivel = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        }))
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Body Stiffness"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(100.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(5.0f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().bodyStiffness)
                        ->Bind(&bodyStiffnessSlider)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.bodyStiffness = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        }))
                        ->AsShared()
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
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.shoulderRotationWeight = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        }))
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Wrist Twist Fix Amount"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(80.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(0.8f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().wristTwistFixAmount)
                        ->Bind(&wristTwistFixSlider)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.wristTwistFixAmount = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        }))
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Shoulder Twist Fix Amount"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(80.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(0.8f)
                        ->SetValue(Config::ConfigManager::GetIKSettings().shoulderTwistFixAmount)
                        ->Bind(&shoulderTwistFixSlider)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            auto settings = Config::ConfigManager::GetIKSettings();
                            settings.shoulderTwistFixAmount = val;
                            Config::ConfigManager::SetIKSettings(settings);
                            AvatarManager::UpdateVRIK();
                        }))
                        ->AsShared()
                })
            }
        );
    }

    void AvatarSettingsViewController::UpdateIKTab()
    {
        legSwivelSlider->SetValue(Config::ConfigManager::GetIKSettings().legSwivel);
        armSwivelSlider->SetValue(Config::ConfigManager::GetIKSettings().armSwivel);
        bodyStiffnessSlider->SetValue(Config::ConfigManager::GetIKSettings().bodyStiffness);
        shoulderRotationWeightSlider->SetValue(Config::ConfigManager::GetIKSettings().shoulderRotationWeight);
        wristTwistFixSlider->SetValue(Config::ConfigManager::GetIKSettings().wristTwistFixAmount);
        shoulderTwistFixSlider->SetValue(Config::ConfigManager::GetIKSettings().shoulderTwistFixAmount);
    }

    std::shared_ptr<CP_SDK::XUI::XUISlider> AvatarSettingsViewController::BuildFingerSlider(const int finger)
    {
        int idx = finger;
        auto slider = CP_SDK::XUI::XUISlider::Make()
            ->SetIncrements(180.0f)
            ->SetMinValue(-90)
            ->SetMaxValue(90)
            ->SetInteger(true)
            ->SetValue(GetValue(Config::ConfigManager::GetFingerPosingSettings().grabPose, idx))
            ->OnValueChanged(CP_SDK::Utils::Action<float>([this, idx](const float val)
            {
                auto settings = Config::ConfigManager::GetFingerPosingSettings();
                settings.grabPose = SetValue(settings.grabPose, idx, val);
                Config::ConfigManager::SetFingerPosingSettings(settings);
                AvatarManager::SetFingerPose(Config::ConfigManager::GetFingerPosingSettings().grabPose);
            }))
            ->AsShared();
        fingerSliders[idx] = slider;
        return slider;
    }

    // very longggg
    std::shared_ptr<CP_SDK::XUI::XUITabControl> AvatarSettingsViewController::BuildFingerPoseSettingsTab()
    {
        return CP_SDK::XUI::XUITabControl::Make({
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
                            }),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Little Proximal Horizontal Angle"),
                                BuildFingerSlider(3)
                            })
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
                            }),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Ring Proximal Horizontal Angle"),
                                BuildFingerSlider(7)
                            })
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
                            }),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Middle Proximal Horizontal Angle"),
                                BuildFingerSlider(11)
                            })
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
                            }),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Index Proximal Horizontal Angle"),
                                BuildFingerSlider(15)
                            })
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
                            }),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Thumb Proximal Horizontal Angle"),
                                BuildFingerSlider(19)
                            })
                        })
                    }
                });
    }

    void AvatarSettingsViewController::UpdateFingerPosingTab()
    {
        for (int i = 0; i < fingerSliders.size(); ++i)
        {
            const auto slider = fingerSliders[i];
            slider->SetValue(GetValue(Config::ConfigManager::GetFingerPosingSettings().grabPose, i));
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
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                        {
                            auto settings = Config::ConfigManager::GetLocomotionSettings();
                            settings.footDistance = val;
                            Config::ConfigManager::SetLocomotionSettings(settings);
                            AvatarManager::UpdateVRIK();
                        }))
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Step Threshold"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(30.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(0.3f)
                        ->SetValue(Config::ConfigManager::GetLocomotionSettings().stepThreshold)
                        ->Bind(&stepThresholdSlider)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                        {
                            auto settings = Config::ConfigManager::GetLocomotionSettings();
                            settings.stepThreshold = val;
                            Config::ConfigManager::SetLocomotionSettings(settings);
                            AvatarManager::UpdateVRIK();
                        }))
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
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                        {
                            auto settings = Config::ConfigManager::GetLocomotionSettings();
                            settings.stepHeight = val;
                            Config::ConfigManager::SetLocomotionSettings(settings);
                            AvatarManager::UpdateVRIK();
                        }))
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Step Offset Z"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(40.0f)
                        ->SetMinValue(-0.2f)
                        ->SetMaxValue(0.2f)
                        ->SetValue(Config::ConfigManager::GetLocomotionSettings().stepOffset.z)
                        ->Bind(&stepOffsetZSlider)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                        {
                            auto settings = Config::ConfigManager::GetLocomotionSettings();
                            settings.stepOffset.z = val;
                            Config::ConfigManager::SetLocomotionSettings(settings);
                            AvatarManager::UpdateVRIK();
                        }))
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
                    ->OnValueChanged(CP_SDK::Utils::Action<UnityEngine::Color>([](UnityEngine::Color val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.globalColor = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Light Intensity"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(2.5f)
                    ->SetValue(Config::ConfigManager::GetLightingSettings().globalLightIntensity)
                    ->Bind(&globalIntensitySlider)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.globalLightIntensity = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Light Rotation X"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetIncrements(360.0f)
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(360.0f)
                    ->SetValue(Config::ConfigManager::GetLightingSettings().lightRotation.x)
                    ->Bind(&lightRotationXSlider)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.lightRotation.x = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Light Rotation Y"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetIncrements(360.0f)
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(360.0f)
                    ->SetValue(Config::ConfigManager::GetLightingSettings().lightRotation.y)
                    ->Bind(&lightRotationYSlider)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.lightRotation.y = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
                    ->AsShared()
            }),

            CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"Beatmap Lighting"),
                CP_SDK::XUI::XUIToggle::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLighting)
                    ->Bind(&beatmapLightingToggle)
                    ->OnValueChanged(CP_SDK::Utils::Action<bool>([](bool val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.beatmapLighting = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"BM Lighting Brightness"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLightingBrightness)
                    ->Bind(&beatmapLightingBrightnessSlider)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.beatmapLightingBrightness = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"BM Lighting Saturation"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLightingColorIntensity)
                    ->Bind(&beatmapLightingColorIntensitySlider)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.beatmapLightingColorIntensity = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"BM Lighting Min Brightness"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().beatmapLightingMinimumBrightness)
                    ->Bind(&beatmapLightingMinBrightnessSlider)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.beatmapLightingMinimumBrightness = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
                    ->AsShared(),
            }),

            CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"Saber Lighting"),
                CP_SDK::XUI::XUIToggle::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().saberLighting)
                    ->Bind(&saberLightingToggle)
                    ->OnValueChanged(CP_SDK::Utils::Action<bool>([](bool val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.saberLighting = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Saber Lighting Range"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().saberLightingRange)
                    ->Bind(&saberLightingRangeSlider)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.saberLightingRange = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Saber Lighting Intensity"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetValue(Config::ConfigManager::GetLightingSettings().saberLightingIntensity)
                    ->Bind(&saberLightingIntensitySlider)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        auto settings = Config::ConfigManager::GetLightingSettings();
                        settings.saberLightingIntensity = val;
                        Config::ConfigManager::SetLightingSettings(settings);
                        LightManager::UpdateLightValues();
                    }))
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
        saberLightingToggle->SetValue(Config::ConfigManager::GetLightingSettings().saberLighting);
    }

}
