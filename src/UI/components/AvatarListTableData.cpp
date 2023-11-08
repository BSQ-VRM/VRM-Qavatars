#include "UI/components/AvatarListTableData.hpp"

#include "System/Action_2.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "custom-types/shared/delegate.hpp"
#include "UI/components/AvatarListTableCell.hpp"

DEFINE_TYPE(VRMQavatars::UI::Components, AvatarListTableData);

namespace VRMQavatars::UI::Components
{
    void AvatarListTableData::ctor()
    {
        INVOKE_CTOR();
        reuseIdentifier = StringW("AvatarCellList");
        cellSize = 12.0f;
    }

    void AvatarListTableData::Start()
    {
        using DelegateType = System::Action_2<HMUI::TableView*, int>;
        std::function<void(HMUI::TableView*, int)> fun = std::bind(&AvatarListTableData::DidSelectCellWithIdx, this, std::placeholders::_1, std::placeholders::_2);
        auto delegate = custom_types::MakeDelegate<DelegateType*>(fun);
        tableView->add_didSelectCellWithIdxEvent(delegate);
    }

    void AvatarListTableData::DidSelectCellWithIdx(HMUI::TableView* tableView, int idx)
    {
        if (onSelect)
        {
            auto& desc = *std::next(avatarDescriptors.begin(), idx);
            ListWrapper<AvatarListTableCell*> visibleCells(reinterpret_cast<List<AvatarListTableCell*>*>(tableView->visibleCells));

            auto cell = std::find_if(visibleCells.begin(), visibleCells.end(), [desc](auto x)
                                     { return desc.filePath == x->descriptor.filePath; });
            if (cell != visibleCells.end())
                (*cell)->Select();
        }
    }

    float AvatarListTableData::CellSize()
    {
        return cellSize;
    }

    int AvatarListTableData::NumberOfCells()
    {
        return avatarDescriptors.size();
    }

    HMUI::TableCell* AvatarListTableData::CellForIdx(HMUI::TableView* tableView, int idx)
    {
        auto tableCell = reinterpret_cast<AvatarListTableCell*>(tableView->DequeueReusableCellForIdentifier(reuseIdentifier));

        if (!tableCell)
        {
            tableCell = AvatarListTableCell::CreateNewCell();
        }

        tableCell->tableData = this;
        tableCell->set_reuseIdentifier(reuseIdentifier);
        tableCell->SetDescriptor(*std::next(avatarDescriptors.begin(), idx));
        if (onSelect)
            tableCell->onSelect = onSelect;

        return tableCell;
    }
}