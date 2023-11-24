#include "UI/modals/AvatarSelectionModal.hpp"

#include "UnityEngine/Application.hpp"

#include "AvatarManager.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/XUI/Templates.hpp"
#include "config/ConfigManager.hpp"

#include "UI/AvatarSelectionViewController.hpp"
#include "UI/AvatarSettingsViewController.hpp"

namespace VRMQavatars::UI::Modals {

    CP_SDK_IL2CPP_INHERIT_INIT(AvatarSelectionModal);
    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(AvatarSelectionModal)
    {
        OnShow  = {this, &AvatarSelectionModal::Impl_OnShow};
        OnClose = {this, &AvatarSelectionModal::Impl_OnClose};
    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(AvatarSelectionModal)
    {

    }

    struct usageInfo
    {
        std::string usage = "";
        std::string violent = "";
        std::string sexual = "";
        std::string commercial = "";

        void load(const VRMDescriptor& descriptor)
        {
            //WHY
            if(descriptor.vrm0.has_value())
            {
                switch(descriptor.vrm0.value().meta.allowedUserName)
                {
                    case VRMC_VRM_0_0::Meta::AllowedUserName::Everyone:
                        usage = "<color=#44d437>Everyone";
                        break;
                    case VRMC_VRM_0_0::Meta::AllowedUserName::ExplicitlyLicensedPerson:
                        usage = "<color=#d4cc37>Explicitly Licensed Person";
                        break;
                    case VRMC_VRM_0_0::Meta::AllowedUserName::OnlyAuthor:
                        usage = "<color=#9c1d14>Only Author";
                        break;
                    default:
                        break;
                }

                switch(descriptor.vrm0.value().meta.violentUssageName)
                {
                    case VRMC_VRM_0_0::Meta::ViolentUssageName::Allow:
                        violent = "<color=#44d437>Allowed";
                        break;
                    case VRMC_VRM_0_0::Meta::ViolentUssageName::Disallow:
                        violent = "<color=#9c1d14>Disallowed";
                        break;
                    default:
                        break;
                }

                switch(descriptor.vrm0.value().meta.sexualUssageName)
                {
                    case VRMC_VRM_0_0::Meta::SexualUssageName::Allow:
                        sexual = "<color=#44d437>Allowed";
                        break;
                    case VRMC_VRM_0_0::Meta::SexualUssageName::Disallow:
                        sexual = "<color=#9c1d14>Disallowed";
                        break;
                    default:
                        break;
                }

                switch(descriptor.vrm0.value().meta.commercialUssageName)
                {
                    case VRMC_VRM_0_0::Meta::CommercialUssageName::Allow:
                        commercial = "<color=#44d437>Allowed";
                        break;
                    case VRMC_VRM_0_0::Meta::CommercialUssageName::Disallow:
                        commercial = "<color=#9c1d14>Disallowed";
                        break;
                    default:
                        break;
                }
            }
        }
    };

    void AvatarSelectionModal::Impl_OnShow()
    {
        if(usageText != nullptr)
        {
            return;
        }
        auto usage = usageInfo();
        usage.load(activeDescriptor);

        CP_SDK::XUI::Templates::ModalRectLayout({
            CP_SDK::XUI::XUIVLayout::Make({
                CP_SDK::XUI::XUIText::Make(u"<b><color=#6a5cd6>Avatar License")
                    ->SetFontSize(8)
                    ->AsShared(),
                CP_SDK::XUI::XUIHLayout::Make(
                {
                    CP_SDK::XUI::XUIText::Make(u"Who can use this avatar?"),
                    CP_SDK::XUI::XUIText::Make(u"<b>Everyone")
                        ->SetText(to_utf16("<b>" + usage.usage))
                        ->Bind(&usageText)
                        ->AsShared()
                })
                ->OnReady([](CP_SDK::UI::Components::CHLayout* layout)
                {
                    layout->CSizeFitter()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
                })
                ->AsShared(),
                CP_SDK::XUI::XUIHLayout::Make(
                {
                    CP_SDK::XUI::XUIText::Make(u"Perform Violent Acts?"),
                    CP_SDK::XUI::XUIText::Make(u"<b>Disallow")
                        ->SetText(to_utf16("<b>" + usage.violent))
                        ->Bind(&violentText)
                        ->AsShared()
                }),
                CP_SDK::XUI::XUIHLayout::Make(
                {
                    CP_SDK::XUI::XUIText::Make(u"Perform Sexual Acts?"),
                    CP_SDK::XUI::XUIText::Make(u"<b>Disallow")
                        ->SetText(to_utf16("<b>" + usage.sexual))
                        ->Bind(&sexualText)
                        ->AsShared()
                }),
                CP_SDK::XUI::XUIHLayout::Make(
                {
                    CP_SDK::XUI::XUIText::Make(u"Allow Commerical Use?"),
                    CP_SDK::XUI::XUIText::Make(u"<b>Disallow")
                        ->SetText(to_utf16("<b>" + usage.commercial))
                        ->Bind(&commericalText)
                        ->AsShared()
                }),
                CP_SDK::XUI::XUIHLayout::Make(
                {
                    CP_SDK::XUI::XUIPrimaryButton::Make(u"<b>Read More: License")
                        ->OnClick([this]
                        {
                            UnityEngine::Application::OpenURL(activeDescriptor.vrm0.value().meta.otherLicenseUrl);
                        })
                        ->AsShared(),
                    CP_SDK::XUI::XUIPrimaryButton::Make(u"<b>Read More: Permissions")
                        ->OnClick([this]
                        {
                            UnityEngine::Application::OpenURL(activeDescriptor.vrm0.value().meta.otherPermissionUrl);
                        })
                        ->AsShared(),
                    CP_SDK::XUI::XUIPrimaryButton::Make(u"<b><color=#d65c64>Close")
                    ->OnClick([this]
                        {
                            VController->CloseModal(this);
                        })
                        ->AsShared(),
                    CP_SDK::XUI::XUIPrimaryButton::Make(u"<b><color=#426b8c>Agree & Calibrate model")
                        ->OnClick([this]
                        {
                            VController->CloseModal(this);
                            Config::ConfigManager::GetAvatarConfig().HasAgreedToTerms.SetValue(true);
                            VController->GetComponent<ViewControllers::AvatarSelectionViewController*>()->Calibrate();
                        })
                        ->AsShared(),
                })
            })
        })
        ->BuildUI(get_transform());
    }

    void AvatarSelectionModal::Impl_OnClose()
    {

    }

    void AvatarSelectionModal::SetInfo(const VRMDescriptor& descriptor)
    {
        activeDescriptor = descriptor;
        if(usageText != nullptr)
        {
            auto usage = usageInfo();
            usage.load(activeDescriptor);

            usageText->SetText(to_utf16(usage.usage));
            violentText->SetText(to_utf16(usage.violent));
            sexualText->SetText(to_utf16(usage.sexual));
            commericalText->SetText(to_utf16(usage.commercial));
        }
    }
}
