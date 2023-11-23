#include "UI/components/AvatarListItem.hpp"

#include "UI/components/AvatarListCell.hpp"

namespace VRMQavatars::UI::Components {

    AvatarListItem::AvatarListItem(const VRMDescriptor& descriptor)
    {
        this->descriptor = descriptor;
    }

    AvatarListItem::~AvatarListItem()
    {

    }

    void AvatarListItem::OnShow()
    {
        const auto l_TextListCell = GetCellAsClassOf<AvatarListCell*>();
        if (!l_TextListCell)
            return;

        l_TextListCell->NameText->Element()->TMProUGUI()->set_text(descriptor.name);
        l_TextListCell->AuthorText->Element()->TMProUGUI()->set_text(descriptor.author);
        l_TextListCell->FilenameText->Element()->TMProUGUI()->set_text(descriptor.filePath);
        l_TextListCell->Tooltip = to_utf16("Calibration Status: Uncalibrated");
    }

}
