#pragma once
#include <chatplex-sdk-bs/shared/CP_SDK/XUI/XUI.hpp>

#include "chatplex-sdk-bs/shared/CP_SDK/UI/IModal.hpp"
#include "vrm/VRMDescriptor.hpp"

namespace VRMQavatars::UI::Modals
{
    class AvatarSelectionModal : public CP_SDK::UI::IModal
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.UI.Modals", AvatarSelectionModal, IModal);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(AvatarSelectionModal);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(AvatarSelectionModal);

        void Impl_OnShow();
        void Impl_OnClose();

        std::shared_ptr<CP_SDK::XUI::XUIText> usageText;
        std::shared_ptr<CP_SDK::XUI::XUIText> violentText;
        std::shared_ptr<CP_SDK::XUI::XUIText> sexualText;
        std::shared_ptr<CP_SDK::XUI::XUIText> commericalText;
        VRMDescriptor activeDescriptor;

    public:
        void SetInfo(const VRMDescriptor& descriptor);
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::UI::Modals::AvatarSelectionModal);