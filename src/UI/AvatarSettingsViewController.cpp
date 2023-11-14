#include "UI/AvatarSettingsViewController.hpp"

#include <HandController.hpp>
#include <sstream>

#include "LightManager.hpp"

#include "assets.hpp"
#include "AvatarManager.hpp"
#include "customTypes/TargetManager.hpp"

#include <string_view>

#include "chatplex-sdk-bs/shared/CP_SDK/XUI/Templates.hpp"

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
        CP_SDK::XUI::Templates::FullRectLayoutMainView(
            {
                CP_SDK::XUI::Templates::TitleBar(u"Avatar Settings")->Make(),
                CP_SDK::XUI::XUITabControl::Make(u"Settings Tab", {
                    { u"Calibration", BuildCalibrationTab() },
                    { u"Hands", BuildHandOffsetsTab() },
                    { u"Fingers", BuildFingerPoseSettingsTab() },
                    { u"IK", BuildIKSettingsTab() },
                    { u"Locomotion", BuildLocoSettingsTab() },
                    { u"Lighting", BuildLightingTab() },
                })
            }
        )
        ->BuildUI(get_transform());
    }

    void AvatarSettingsViewController::UpdatePos()
    {
        AvatarManager::SetHandOffset(offsetPose);
    }

    int GetValue(const std::string& pose, const int idx)
    {
        const auto values = HandController::ParseRotations(HandController::SplitPose(pose));
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
                CP_SDK::XUI::XUIVSpacer::Make(0.1f),

                CP_SDK::XUI::XUIText::Make(u"Calibration Type"),
                CP_SDK::XUI::XUIDropdown::Make({ u"Match Armspans", u"Match Heights", u"Fixed" })
                    ->SetValue(u"Match Armspans")
                    ->AsShared(),

                CP_SDK::XUI::XUIVSpacer::Make(0.1f),

                CP_SDK::XUI::XUIText::Make(u"Fixed Scale"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(0.2f)
                    ->SetMaxValue(2.0f)
                    ->SetValue(1.0f)
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
                        ->SetValue(offsetPose.posX)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            offsetPose.posX = val;
                            UpdatePos();
                        }))
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Y Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 1.0f, 0.0f, 1.0f})
                        ->SetIncrements(20.0f)
                        ->SetMinValue(-0.5f)
                        ->SetMaxValue(0.5f)
                        ->SetValue(offsetPose.posY)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            offsetPose.posY = val;
                            UpdatePos();
                        }))
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Z Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 0.0f, 1.0f, 1.0f})
                        ->SetIncrements(20.0f)
                        ->SetMinValue(-0.5f)
                        ->SetMaxValue(0.5f)
                        ->SetValue(offsetPose.posZ)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            offsetPose.posZ = val;
                            UpdatePos();
                        }))
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
                        ->SetValue(offsetPose.rotX)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            offsetPose.rotX = val;
                            UpdatePos();
                        }))
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Y Rotation"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 1.0f, 0.0f, 1.0f})
                        ->SetIncrements(180.0f)
                        ->SetMinValue(-90.0f)
                        ->SetMaxValue(90.0f)
                        ->SetValue(offsetPose.rotY)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            offsetPose.rotY = val;
                            UpdatePos();
                        }))
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Z Rotation"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 0.0f, 1.0f, 1.0f})
                        ->SetIncrements(180.0f)
                        ->SetMinValue(-90.0f)
                        ->SetMaxValue(90.0f)
                        ->SetValue(offsetPose.rotZ)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            offsetPose.rotZ = val;
                            UpdatePos();
                        }))
                        ->AsShared()
                })
            })
            ->SetSpacing(10.0f)
            ->AsShared();
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

        // very longggg
        std::shared_ptr<CP_SDK::XUI::XUITabControl> AvatarSettingsViewController::BuildFingerPoseSettingsTab()
        {
            return CP_SDK::XUI::XUITabControl::Make({
                        {
                            u"Little",
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Little Distal"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 0))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 0, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Little Intermediate"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 1))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 1, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Little Proximal"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 2))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 2, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Little Proximal Horizontal Angle"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 3))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 3, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),
                            })
                        },
                        {
                            u"Ring",
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Ring Distal"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 4))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 4, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Ring Intermediate"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 5))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 5, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Ring Proximal"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 6))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 6, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Ring Proximal Horizontal Angle"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 7))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 7, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),
                            })
                        },
                        {
                            u"Middle",
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Middle Distal"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 8))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 8, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Middle Intermediate"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 9))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 9, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Middle Proximal"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 10))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 10, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Middle Proximal Horizontal Angle"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 11))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 11, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),
                            })
                        },
                        {
                            u"Index",
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Index Distal"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 12))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 12, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Index Intermediate"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 13))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 13, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Index Proximal"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 14))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 14, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Index Proximal Horizontal Angle"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 15))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 15, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),
                            })
                        },
                        {
                            u"Thumb",
                            CP_SDK::XUI::XUIVLayout::Make({
                                CP_SDK::XUI::XUIText::Make(u"Thumb Distal"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 16))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 16, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Thumb Intermediate"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 17))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 17, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Thumb Proximal"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 18))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 18, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),

                                CP_SDK::XUI::XUIText::Make(u"Thumb Proximal Horizontal Angle"),
                                CP_SDK::XUI::XUISlider::Make()
                                    ->SetIncrements(180.0f)
                                    ->SetMinValue(-90)
                                    ->SetMaxValue(90)
                                    ->SetInteger(true)
                                    ->SetValue(GetValue(pose, 19))
                                    ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val)
                                    {
                                        pose = SetValue(pose, 19, val);
                                        AvatarManager::SetFingerPose(pose);
                                    }))
                                    ->AsShared(),
                            })
                        }
                    });
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
            }
        );
    }
}