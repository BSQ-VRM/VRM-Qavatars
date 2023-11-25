#pragma once

#include "chatplex-sdk-bs/shared/CP_SDK/UI/Data/IListCell.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/UI/Components/CText.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/XUI/XUI.hpp"

namespace VRMQavatars::UI::Components {

    class AvatarListCell : public CP_SDK::UI::Data::IListCell
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.UI.Components", AvatarListCell, IListCell);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(AvatarListCell);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(AvatarListCell);
    public:
        std::shared_ptr<CP_SDK::XUI::XUIText> NameText;
        std::shared_ptr<CP_SDK::XUI::XUIText> AuthorText;
        std::shared_ptr<CP_SDK::XUI::XUIText> FilenameText;
        std::shared_ptr<CP_SDK::XUI::XUIImage> Image;
    private:
        void Build_Impl();
        float GetCellHeight_Impl();
        IListCell* AddSelfComponent_Impl(UnityEngine::GameObject* p_Target);
    };

}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::UI::Components::AvatarListCell);