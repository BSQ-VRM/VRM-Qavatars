#include "UI/AvatarSelectionViewController.hpp"

#include "HMUI/TableView_ScrollPositionType.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include "UnityEngine/WaitForSeconds.hpp"

#include "bsml/shared/BSML.hpp"

#include "assets.hpp"
#include "utils/FileUtils.hpp"
#include "vrm/VRMDescriptor.hpp"
#include "AssetLib/modelImporter.hpp"
#include "AvatarManager.hpp"

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
    if (firstActivation)
    {
        BSML::parse_and_construct(IncludedAssets::avatarSelectionView_bsml, get_transform(), this);
    }
    Refresh();
}

int VRMQavatars::UI::ViewControllers::AvatarSelectionViewController::GetSelectedCellIdx()
{
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
    std::vector<std::string> vrms = {};
    if(GetFilesInFolderPath("vrm", vrm_path, vrms))
    {
        auto& avatarSet = avatarList->avatarDescriptors;
        int row = GetSelectedCellIdx();
        for(auto& vrm : vrms)
        {
            avatarSet.emplace(LoadVRMDescriptor(vrm));
        }
        auto tableView = avatarList->tableView;
        tableView->ReloadData();
        tableView->RefreshCells(true, true);
        tableView->ScrollToCellWithIdx(std::clamp(row, 0, (int)avatarSet.size() - 1), HMUI::TableView::ScrollPositionType::Center, true);
        tableView->ClearSelection();
    }
}

void VRMQavatars::UI::ViewControllers::AvatarSelectionViewController::PostParse()
{
    auto tableView = avatarListTableData->tableView;
    auto go = avatarListTableData->get_gameObject();
    Object::DestroyImmediate(avatarListTableData);
    avatarListTableData = nullptr;
    avatarList = go->AddComponent<VRMQavatars::UI::Components::AvatarListTableData*>();
    avatarList->tableView = tableView;
    tableView->SetDataSource(reinterpret_cast<HMUI::TableView::IDataSource*>(avatarList), false);

    avatarList->onSelect = std::bind(reinterpret_cast<void (AvatarSelectionViewController::*)(HMUI::TableCell*)>(&AvatarSelectionViewController::OnSelectAvatar), this, std::placeholders::_1);
}

void VRMQavatars::UI::ViewControllers::AvatarSelectionViewController::OnSelectAvatar(VRMQavatars::UI::Components::AvatarListTableCell* cell)
{
    if (cell)
    {
        auto ctx = AssetLib::ModelImporter::LoadVRM(vrm_path + std::string("/") + cell->descriptor.filePath, AssetLib::ModelImporter::mtoon.ptr());
        VRMQavatars::AvatarManager::SetContext(ctx);
    }
}

#define coro(...) custom_types::Helpers::CoroutineHelper::New(__VA_ARGS__)

custom_types::Helpers::Coroutine StartCalibration()
{
    co_yield coro(UnityEngine::WaitForSeconds::New_ctor(4.0f));
    VRMQavatars::AvatarManager::Calibrate();
    co_return;
}

void VRMQavatars::UI::ViewControllers::AvatarSelectionViewController::Calibrate()
{
    StartCoroutine(coro(StartCalibration()));
}