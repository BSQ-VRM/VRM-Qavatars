#pragma once
#include "chatplex-sdk-bs/shared/CP_SDK/UI/IModal.hpp"

namespace VRMQavatars::UI::Modals {

    class IndividualConfigModal : public CP_SDK::UI::IModal
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.UI.Modals", IndividualConfigModal, IModal);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(IndividualConfigModal);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(IndividualConfigModal);

        void Impl_OnShow();
        void Impl_OnClose();

        public:
            void Init();

    };

}   ///< namespace CP_SDK::UI::Modals

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::UI::Modals::IndividualConfigModal);