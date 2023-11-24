#pragma once
#include <chatplex-sdk-bs/shared/CP_SDK/XUI/XUI.hpp>

#include "chatplex-sdk-bs/shared/CP_SDK/UI/IModal.hpp"

namespace VRMQavatars::UI::Modals {

    class IndividualConfigModal : public CP_SDK::UI::IModal
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.UI.Modals", IndividualConfigModal, IModal);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(IndividualConfigModal);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(IndividualConfigModal);

        void Impl_OnShow();
        void Impl_OnClose();

        std::shared_ptr<CP_SDK::XUI::XUIToggle> offsetToggle;
        std::shared_ptr<CP_SDK::XUI::XUIToggle> fingerToggle;
        std::shared_ptr<CP_SDK::XUI::XUIToggle> ikToggle;
        std::shared_ptr<CP_SDK::XUI::XUIToggle> locoToggle;
        std::shared_ptr<CP_SDK::XUI::XUIToggle> lightToggle;
        std::shared_ptr<CP_SDK::XUI::XUIToggle> triggerToggle;
    };

}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::UI::Modals::IndividualConfigModal);