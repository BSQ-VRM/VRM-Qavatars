#include "UI/AvatarSelectionViewController.hpp"

#include "HMUI/TableView_ScrollPositionType.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"

#include "bsml/shared/BSML.hpp"

#include "assets.hpp"
#include "utils/FileUtils.hpp"
#include "vrm/VRMDescriptor.hpp"

#include <fstream>

DEFINE_TYPE(VRMQavatars::UI::ViewControllers, AvatarSelectionViewController);

bool is_whitespace(const std::string& s) {
    return std::all_of(s.begin(), s.end(), isspace);
}

VRMQavatars::VRMDescriptor LoadVRMDescriptor(const std::string& path) {
    VRMQavatars::VRMDescriptor descriptor;
    
    //Load in binary to parse out VRM data

    std::ifstream binFile(vrm_path + std::string("/") + path, std::ios::binary);

    binFile.seekg(12); //Skip past the 12 byte header, to the json header
    uint32_t jsonLength;
    binFile.read((char*)&jsonLength, sizeof(uint32_t)); //Read the length of the json file from it's header

    std::string jsonStr;
    jsonStr.resize(jsonLength);
    binFile.seekg(20); // Skip the rest of the JSON header to the start of the string
    binFile.read(jsonStr.data(), jsonLength); // Read out the json string

    auto doc = nlohmann::json::parse(jsonStr);
    auto exts = doc["extensions"];
    if(exts.contains("VRM"))
    {
        VRMC_VRM_0_0::Vrm vrm;
        VRMC_VRM_0_0::from_json(exts["VRM"], vrm);
        descriptor.vrm0 = vrm;
    }
    if(exts.contains("VRMC_vrm"))
    {
        VRMC_VRM_1_0::Vrm vrm;
        VRMC_VRM_1_0::from_json(exts["VRMC_vrm"], vrm);
        descriptor.vrm1 = vrm;
    }

    descriptor.filePath = path;

    if(descriptor.vrm0.has_value())
    {
        descriptor.name = descriptor.vrm0.value().meta.title;
        descriptor.name = is_whitespace(descriptor.name) ? path : descriptor.name;
        descriptor.author = descriptor.vrm0.value().meta.author;
    }

    if(descriptor.vrm1.has_value())
    {
        descriptor.name = descriptor.vrm1.value().meta.name;
        descriptor.name = is_whitespace(descriptor.name) ? path : descriptor.name;
        descriptor.author = descriptor.vrm1.value().meta.authors[0]; //TODO: join with comma
    }

    return descriptor;
}

void VRMQavatars::UI::ViewControllers::AvatarSelectionViewController::DidActivate(bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling) {
    getLogger().info("x7");
    if (firstActivation)
    {
        getLogger().info("x8");
        BSML::parse_and_construct(IncludedAssets::avatarSelectionView_bsml, get_transform(), this);
        getLogger().info("x9");
    }
    Refresh();
}

int VRMQavatars::UI::ViewControllers::AvatarSelectionViewController::GetSelectedCellIdx()
{
    getLogger().info("x10");
    if (!avatarList || !avatarList->m_CachedPtr.m_value)
        return -1;
    auto tableView = avatarList->tableView;
    auto enumerator = tableView->selectedCellIdxs->GetEnumerator();
    int result = -1;
    if (enumerator.MoveNext())
        result = enumerator.get_Current();
    enumerator.Dispose();
    return result;
}

void VRMQavatars::UI::ViewControllers::AvatarSelectionViewController::Refresh()
{
    getLogger().info("x11");
    std::vector<std::string> vrms = {};
    if(GetFilesInFolderPath("vrm", vrm_path, vrms))
    {
        getLogger().info("x12");
        auto& avatarSet = avatarList->avatarDescriptors;
        int row = GetSelectedCellIdx();
        getLogger().info("x13");
        for(auto& vrm : vrms)
        {
            avatarSet.emplace(LoadVRMDescriptor(vrm));
        }
        getLogger().info("x14");
        getLogger().info("x15");
        auto tableView = avatarList->tableView;
        getLogger().info("x16");
        tableView->ReloadData();
        tableView->RefreshCells(true, true);
        tableView->ScrollToCellWithIdx(std::clamp(row, 0, (int)avatarSet.size() - 1), HMUI::TableView::ScrollPositionType::Center, true);
        tableView->ClearSelection();
        getLogger().info("x17");
    }
}

void VRMQavatars::UI::ViewControllers::AvatarSelectionViewController::PostParse()
{
    getLogger().info("x18");
    auto tableView = avatarListTableData->tableView;
    auto go = avatarListTableData->get_gameObject();
    Object::DestroyImmediate(avatarListTableData);
    getLogger().info("x19");
    avatarListTableData = nullptr;
    avatarList = go->AddComponent<VRMQavatars::UI::Components::AvatarListTableData*>();
    getLogger().info("x20");
    avatarList->tableView = tableView;
    tableView->SetDataSource(reinterpret_cast<HMUI::TableView::IDataSource*>(avatarList), false);
    getLogger().info("x21");

    avatarList->onSelect = std::bind(reinterpret_cast<void (AvatarSelectionViewController::*)(HMUI::TableCell*)>(&AvatarSelectionViewController::OnSelectAvatar), this, std::placeholders::_1);
    getLogger().info("x22");
}

void VRMQavatars::UI::ViewControllers::AvatarSelectionViewController::OnSelectAvatar(VRMQavatars::UI::Components::AvatarListTableCell* cell)
{
    getLogger().info("x23");
    if (cell)
    {
    }
}