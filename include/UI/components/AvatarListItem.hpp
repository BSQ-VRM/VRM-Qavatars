#pragma once

#include "chatplex-sdk-bs/shared/CP_SDK/UI/Data/IListItem.hpp"

#include <TMPro/TextAlignmentOptions.hpp>

#include "vrm/VRMDescriptor.hpp"

namespace VRMQavatars::UI::Components {

    class AvatarListItem : public CP_SDK::UI::Data::IListItem
    {
        CP_SDK_NO_DEF_CTORS(AvatarListItem);

        public:
            VRMDescriptor descriptor;

            AvatarListItem(const VRMDescriptor& descriptor);
            virtual ~AvatarListItem();

            virtual void OnShow() override;
    };
}
