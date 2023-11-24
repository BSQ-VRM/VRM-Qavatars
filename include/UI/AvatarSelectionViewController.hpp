#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK_BS/UI/ViewController.hpp>

#include "main.hpp"
#include "HMUI/ViewController.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "components/AvatarListItem.hpp"
#include "components/AvatarListCell.hpp"
#include "modals/AvatarSelectionModal.hpp"

namespace VRMQavatars::UI::ViewControllers {
    class AvatarSelectionViewController : public CP_SDK_BS::UI::ViewController {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.UI.ViewControllers", AvatarSelectionViewController, CP_SDK_BS::UI::ViewController);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(AvatarSelectionViewController);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(AvatarSelectionViewController);
        SDK_UI_VIEW_CONTROLLER_INSTANCE();
    public:
        void DidActivate();

        void Refresh();

        void OnListSelect(const Components::AvatarListItem::Ptr& p_ListItem);

        void Calibrate();

        std::shared_ptr<CP_SDK::XUI::XUIVVList> avatarList;

        CP_SDK::Utils::MonoPtr<Modals::AvatarSelectionModal> agreementModal;
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::UI::ViewControllers::AvatarSelectionViewController);