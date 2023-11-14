#include "UI/AvatarSettingsViewController.hpp"

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