#include "UI/AvatarSettingsViewController.hpp"
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

    void AvatarSettingsViewController::DidActivate() {
        CP_SDK::XUI::Templates::FullRectLayoutMainView(
            {
                CP_SDK::XUI::Templates::TitleBar(u"Avatar Settings")->Make(),
                CP_SDK::XUI::XUITabControl::Make(u"Settings Tab", {
                    { u"Calibration", BuildCalibrationTab() },
                    { u"Hands", BuildHandOffsetsTab() },
                    { u"IK", BuildIKSettingsTab() },
                    { u"Lighting", BuildLightingTab() },
                })
            }
        )
        ->BuildUI(get_transform());
    }

    void AvatarSettingsViewController::UpdatePos()
    {
        auto tmanager = VRMQavatars::AvatarManager::currentContext->rootGameObject->GetComponent<TargetManager*>();
        tmanager->leftHandPosX = handOffsetX;
        tmanager->leftHandPosY = handOffsetY;
        tmanager->leftHandPosZ = handOffsetZ;

        tmanager->leftHandRotX = handRotX;
        tmanager->leftHandRotY = handRotY;
        tmanager->leftHandRotZ = handRotZ;
    }

    std::shared_ptr<CP_SDK::XUI::XUIVLayout> AvatarSettingsViewController::BuildCalibrationTab()
    {
        return CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIVSpacer::Make(0.1f),

                CP_SDK::XUI::XUIText::Make(u"Calibration Type"),
                CP_SDK::XUI::XUIDropdown::Make({ u"Match Armspans", u"Match Heights" })
                    ->SetValue(u"Match Armspans")
                    ->AsShared(),

                CP_SDK::XUI::XUIVSpacer::Make(0.1f),

                CP_SDK::XUI::XUIText::Make(u"Fixed Scale"),
                CP_SDK::XUI::XUISlider::Make()
                    ->SetMinValue(0.2f)
                    ->SetMaxValue(2.0f)
                    ->SetValue(1.0f)
                    ->SetIncrements(0.1f)
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
                        ->SetBackground(true, UnityEngine::Color(0.7f, 0.55f, 0.85f, 1.0f), true)
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"X Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({1.0f, 0.0f, 0.0f, 1.0f})
                        ->SetIncrements(20.0f)
                        ->SetMinValue(-0.5f)
                        ->SetMaxValue(0.5f)
                        ->SetValue(handOffsetX)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            handOffsetX = val;
                            UpdatePos();
                        }))
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Y Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 1.0f, 0.0f, 1.0f})
                        ->SetIncrements(20.0f)
                        ->SetMinValue(-0.5f)
                        ->SetMaxValue(0.5f)
                        ->SetValue(handOffsetY)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            handOffsetY = val;
                            UpdatePos();
                        }))
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Z Offset"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 0.0f, 1.0f, 1.0f})
                        ->SetIncrements(20.0f)
                        ->SetMinValue(-0.5f)
                        ->SetMaxValue(0.5f)
                        ->SetValue(handOffsetZ)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            handOffsetZ = val;
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
                        ->SetValue(handRotX)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            handRotX = val;
                            UpdatePos();
                        }))
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Y Rotation"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 1.0f, 0.0f, 1.0f})
                        ->SetIncrements(180.0f)
                        ->SetMinValue(-90.0f)
                        ->SetMaxValue(90.0f)
                        ->SetValue(handRotY)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            handRotY = val;
                            UpdatePos();
                        }))
                        ->AsShared(),

                    CP_SDK::XUI::XUIText::Make(u"Z Rotation"),
                    CP_SDK::XUI::XUISlider::Make()
                        ->SetColor({0.0f, 0.0f, 1.0f, 1.0f})
                        ->SetIncrements(180.0f)
                        ->SetMinValue(-90.0f)
                        ->SetMaxValue(90.0f)
                        ->SetValue(handRotZ)
                        ->OnValueChanged(CP_SDK::Utils::Action<float>([this](const float val) {
                            handRotZ = val;
                            UpdatePos();
                        }))
                        ->AsShared()
                })
            })
            ->SetSpacing(10.0f)
            ->AsShared();
        }

        std::shared_ptr<CP_SDK::XUI::XUIHLayout> AvatarSettingsViewController::BuildIKSettingsTab() {
            return CP_SDK::XUI::XUIHLayout::Make(
                {
                    CP_SDK::XUI::XUIVLayout::Make(
                    {
                        CP_SDK::XUI::XUIText::Make(u"Leg Swivel"),
                        CP_SDK::XUI::XUISlider::Make(),

                        CP_SDK::XUI::XUIText::Make(u"Arm Swivel"),
                        CP_SDK::XUI::XUISlider::Make(),

                        CP_SDK::XUI::XUIText::Make(u"Body Stiffness"),
                        CP_SDK::XUI::XUISlider::Make()
                    }),

                    CP_SDK::XUI::XUIVLayout::Make(
                    {
                        CP_SDK::XUI::XUIText::Make(u"Shoulder Height"),
                        CP_SDK::XUI::XUISlider::Make(),

                        CP_SDK::XUI::XUIText::Make(u"Wrist Twist Fix amount"),
                        CP_SDK::XUI::XUISlider::Make(),

                        CP_SDK::XUI::XUIText::Make(u"Shoulder Twist Fix amount"),
                        CP_SDK::XUI::XUISlider::Make()
                    })
                }
            );
        }

        std::shared_ptr<CP_SDK::XUI::XUIHLayout> AvatarSettingsViewController::BuildLightingTab()
        {
            return CP_SDK::XUI::XUIHLayout::Make(
                {
                    CP_SDK::XUI::XUIVLayout::Make(
                    {
                        CP_SDK::XUI::XUIText::Make(u"Global Light Color"),
                        CP_SDK::XUI::XUIColorInput::Make(),

                        CP_SDK::XUI::XUIText::Make(u"Light Intensity"),
                        CP_SDK::XUI::XUISlider::Make(),

                        CP_SDK::XUI::XUIText::Make(u"Light Rotation X"),
                        CP_SDK::XUI::XUISlider::Make()
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