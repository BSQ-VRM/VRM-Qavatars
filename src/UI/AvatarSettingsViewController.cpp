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
            CP_SDK::XUI::Templates::TitleBar(u"Avatar Settings")->AsShared(),
            CP_SDK::XUI::XUIIconButton::Make(QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::settings_png))
                ->OnClick(CP_SDK::Utils::Action<>([this]
                {
                    ShowModal(configModal.Ptr());
                }))
                ->SetWidth(6.0f)
                ->SetHeight(6.0f)
                ->AsShared(),
            CP_SDK::XUI::XUITabControl::Make(u"Settings Tab", {
                { u"Calibration", BuildCalibrationTab() },
                { u"Hands", BuildHandOffsetsTab() },
                { u"Face", BuildFaceTab() },
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
            UpdateHandOffsetsTab();
            UpdateFingerPosingTab();
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

    std::u16string_view convertToU16StringView(const std::string& utf8String) {
        return std::u16string_view(
            reinterpret_cast<const char16_t*>(utf8String.c_str()),
            utf8String.size()
        );
    }

    std::string convertToUTF8(const std::u16string_view& utf16View) {
        // Use std::wstring_convert to convert UTF-16 to UTF-8
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        return converter.to_bytes(utf16View.data(), utf16View.data() + utf16View.size());
    }

    std::shared_ptr<CP_SDK::XUI::XUIVLayout> AvatarSettingsViewController::BuildCalibrationTab()
    {
        return CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIVSpacer::Make(0.1f),

                CP_SDK::XUI::XUIText::Make(u"Calibration Type"),
                CP_SDK::XUI::XUIDropdown::Make({ u"Match Armspans", u"Match Heights", u"Fixed" })
                    ->SetValue(convertToU16StringView(Config::ConfigManager::GetGlobalConfig().CalibrationType.GetValue()))
                    ->OnValueChanged(CP_SDK::Utils::Action<int, std::u16string_view>([](int idx, std::u16string_view val)
                    {
                        Config::ConfigManager::GetGlobalConfig().CalibrationType.SetValue(convertToUTF8(val));
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIVSpacer::Make(0.1f),

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
                    ->SetInteractable(false)
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
                                CP_SDK::XUI::XUIText::Make(u"Facial Expression Events"),
                                CP_SDK::XUI::XUIToggle::Make()
                            })
                            ->SetSpacing(-1.0f)
                            ->AsShared(),
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Mock Eye Movement"),
                                CP_SDK::XUI::XUIToggle::Make(),
                                CP_SDK::XUI::XUIText::Make(u"Default Facial Expression"),
                                CP_SDK::XUI::XUIToggle::Make()
                            })
                            ->SetSpacing(-1.0f)
                            ->AsShared()
                        }),
                        CP_SDK::XUI::XUISecondaryButton::Make(u"Blendshape Triggers")
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
                                ->SetSpacing(-1.0f)
                                ->AsShared(),
                                CP_SDK::XUI::XUIVLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Weight"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                    CP_SDK::XUI::XUIText::Make(u"Framerate"),
                                    CP_SDK::XUI::XUISlider::Make()
                                })
                                ->SetSpacing(-1.0f)
                                ->AsShared()
                            }),
                            CP_SDK::XUI::XUIHLayout::Make({
                                CP_SDK::XUI::XUIVLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Weight aa"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                    CP_SDK::XUI::XUIText::Make(u"Weight E"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                })
                                ->SetSpacing(-1.0f)
                                ->AsShared(),
                                CP_SDK::XUI::XUIVLayout::Make({
                                    CP_SDK::XUI::XUIText::Make(u"Weight ih"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                    CP_SDK::XUI::XUIText::Make(u"Weight oh"),
                                    CP_SDK::XUI::XUISlider::Make(),
                                    CP_SDK::XUI::XUIText::Make(u"Weight ou"),
                                    CP_SDK::XUI::XUISlider::Make()
                                })
                                ->SetSpacing(-1.0f)
                                ->AsShared()
                            })
                        })
                    })
                    ->SetSpacing(-5.0f)
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
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            AvatarManager::SetLegSwivel(val);
                        }))
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Arm Swivel"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(100.0f)
                        ->SetMinValue(-50.0f)
                        ->SetMaxValue(50.0f)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            AvatarManager::SetArmSwivel(val);
                        }))
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Body Stiffness"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(100.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(5.0f)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            AvatarManager::SetBodyStiffness(val);
                        }))
                        ->AsShared()
                }),
                CP_SDK::XUI::XUIVLayout::Make(
                {
                    CP_SDK::XUI::XUIText::Make(u"Shoulder Height"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(10.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(1.0f)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            AvatarManager::SetShoulderRotation(val);
                        }))
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Wrist Twist Fix Amount"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(80.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(0.8f)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            AvatarManager::SetWristFixWeight(val);
                        }))
                        ->AsShared(),
                    CP_SDK::XUI::XUIText::Make(u"Shoulder Twist Fix Amount"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetIncrements(80.0f)
                        ->SetMinValue(0.0f)
                        ->SetMaxValue(0.8f)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val) {
                            AvatarManager::SetShoulderFixWeight(val);
                        }))
                        ->AsShared()
                })
            }
        );
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
                CP_SDK::XUI::XUIText::Make(u"Foot Distance"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetIncrements(50.0f)
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(1.0f)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        AvatarManager::SetFootDist(val);
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Step Threshold"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetIncrements(50.0f)
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(1.0f)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        AvatarManager::SetStepThreshold(val);
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Step Height"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetIncrements(100.0f)
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(1.0f)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        AvatarManager::SetStepHeight(val);
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Step Offset Z"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetIncrements(20.0f)
                    ->SetMinValue(-1.0f)
                    ->SetMaxValue(1.0f)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        AvatarManager::SetStepOffsetZ(val);
                    }))
                    ->AsShared(),
            })
        });
    }

    std::shared_ptr<CP_SDK::XUI::XUIHLayout> AvatarSettingsViewController::BuildLightingTab()
    {
        return CP_SDK::XUI::XUIHLayout::Make(
        {
            CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"Global Light Color"),
                CP_SDK::XUI::XUIColorInput::Make()
                    ->OnValueChanged(CP_SDK::Utils::Action<UnityEngine::Color>([](UnityEngine::Color val)
                    {
                        LightManager::SetGlobalLightColor(val);
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Light Intensity"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(2.5f)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([](float val)
                    {
                        LightManager::SetGlobalLightIntensity(val);
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Light Rotation X"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetIncrements(360.0f)
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(360.0f)
                    ->SetValue(lightRotation.x)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](float val)
                    {
                        lightRotation.x = val;
                        LightManager::SetGlobalLightRotation(lightRotation);
                    }))
                    ->AsShared(),

                CP_SDK::XUI::XUIText::Make(u"Light Rotation Y"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetIncrements(360.0f)
                    ->SetMinValue(0.0f)
                    ->SetMaxValue(360.0f)
                    ->SetValue(lightRotation.y)
                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](float val)
                    {
                        lightRotation.y = val;
                        LightManager::SetGlobalLightRotation(lightRotation);
                    }))
                    ->AsShared()
            }),

            CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"Beatmap Lighting"),
                CP_SDK::XUI::XUIToggle::Make(),

                CP_SDK::XUI::XUIText::Make(u"BM Lighting Brightness"),
                CP_SDK::XUI::XUISlider::Make(),

                CP_SDK::XUI::XUIText::Make(u"BM Lighting Min Brightness"),
                CP_SDK::XUI::XUISlider::Make()
            }),

            CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"Saber Lighting"),
                CP_SDK::XUI::XUIToggle::Make()
            })
        });
    }
}
