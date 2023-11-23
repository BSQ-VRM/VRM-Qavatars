#include "UI/AvatarSelectionViewController.hpp"

#include <string_view>

#include "AssetLib/modelImporter.hpp"
#include "AssetLib/mappings/gLTFImageReader.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/XUI/Templates.hpp"
#include "config/ConfigManager.hpp"
#include "UI/components/AvatarListCell.hpp"
#include "UI/components/AvatarListItem.hpp"

namespace VRMQavatars::UI::ViewControllers {
    CP_SDK_IL2CPP_INHERIT_INIT(AvatarSelectionViewController);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(AvatarSelectionViewController)
    {
        OnViewCreation      = {this, &AvatarSelectionViewController::DidActivate};
    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(AvatarSelectionViewController)
    {

    }

    bool is_whitespace(const std::string& s) {
        return std::all_of(s.begin(), s.end(), isspace);
    }

    VRMDescriptor LoadVRMDescriptor(const std::string& path) {
        VRMDescriptor descriptor;

        //Load in binary to parse out VRM data

        std::ifstream binFile(vrm_path + std::string("/") + path, std::ios::binary);

        binFile.seekg(12); //Skip past the 12 byte header, to the json header
        uint32_t jsonLength;
        binFile.read(reinterpret_cast<char*>(&jsonLength), sizeof(uint32_t)); //Read the length of the json file from it's header

        std::string jsonStr;
        jsonStr.resize(jsonLength);
        binFile.seekg(20); // Skip the rest of the JSON header to the start of the string
        binFile.read(jsonStr.data(), jsonLength); // Read out the json string

        auto doc = nlohmann::json::parse(jsonStr);
        auto exts = doc["extensions"];
        if(exts.contains("VRM"))
        {
            VRMC_VRM_0_0::Vrm vrm;
            from_json(exts["VRM"], vrm);
            descriptor.vrm0 = vrm;
        }
        if(exts.contains("VRMC_vrm"))
        {
            VRMC_VRM_1_0::Vrm vrm;
            from_json(exts["VRMC_vrm"], vrm);
            descriptor.vrm1 = vrm;
        }

        descriptor.thumbnail = gLTFImageReader::ReadImageIndex(jsonLength, binFile, descriptor.vrm0.has_value() ? descriptor.vrm0.value().meta.texture : descriptor.vrm1.value().meta.thumbnailImage);

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

    void AvatarSelectionViewController::Refresh()
    {
        std::vector<std::string> vrms = {};
        if(GetFilesInFolderPath("vrm", vrm_path, vrms))
        {
            std::vector<Components::AvatarListItem::Ptr> avatarSet;
            for(auto& vrm : vrms)
            {
                avatarSet.push_back(std::make_shared<Components::AvatarListItem>(LoadVRMDescriptor(vrm)));
            }
            avatarList->SetListItems(avatarSet);
            //avatarList->OnListItemSelected({this, &AvatarSelectionViewController::OnListSelect}, true);

            auto& globcon = Config::ConfigManager::GetGlobalConfig();
            if(globcon.hasSelected.GetValue())
            {
                auto pred = [&globcon](const std::shared_ptr<CP_SDK::UI::Data::IListItem>& ptr) { return reinterpret_cast<Components::AvatarListItem*>(ptr.get())->descriptor.filePath == globcon.selectedFileName.GetValue(); };
                const auto it = std::find_if(std::begin(avatarSet),
                                             std::end(avatarSet),
                                             pred);
                const auto itemPos = std::distance(std::begin(avatarSet), it);
                const auto item = reinterpret_cast<Components::AvatarListItem*>(avatarSet[itemPos].get());
                getLogger().info("selected %s %s", item->descriptor.name.c_str(), globcon.selectedFileName.GetValue().c_str());
                avatarList->SetSelectedListItem(item->shared_from_this(), false);
            }
        }
    }

    void AvatarSelectionViewController::OnListSelect(const Components::AvatarListItem::Ptr& p_ListItem)
    {
        auto& globcon = Config::ConfigManager::GetGlobalConfig();
        const auto item = reinterpret_cast<Components::AvatarListItem*>(p_ListItem.get());

        const auto path = item->descriptor.filePath;
        getLogger().info("%s", (std::string(vrm_path) + "/" + path).c_str());

        globcon.hasSelected.SetValue(true);
        globcon.selectedFileName.SetValue(path);
    }

    void AvatarSelectionViewController::DidActivate()
    {
        CP_SDK::XUI::Templates::FullRectLayoutMainView({
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUISecondaryButton::Make(u"Refresh")
                    ->OnClick({this, &AvatarSelectionViewController::Refresh})
                    ->AsShared(),
                CP_SDK::XUI::XUIPrimaryButton::Make(u"Recalibrate"),
            })
            ->SetSpacing(8.0f)
            ->AsShared(),
            CP_SDK::XUI::XUIHLayout::Make({
                CP_SDK::XUI::XUIVVList::Make()
                    ->SetListCellPrefab(CP_SDK::UI::Data::ListCellPrefabs<Components::AvatarListCell>::Get())
                    ->Bind(&avatarList)
                    ->AsShared()
            })
            ->SetHeight(65)
            ->SetSpacing(0)
            ->SetPadding(0)
            ->SetBackground(true, std::nullopt, true)
            ->OnReady([](CP_SDK::UI::Components::CHLayout* x) -> void {
                x->CSizeFitter()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
                x->CSizeFitter()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::Unconstrained);
                x->HOrVLayoutGroup()->set_childForceExpandWidth(true);
                x->HOrVLayoutGroup()->set_childForceExpandHeight(true);
                x->LElement()->set_preferredWidth(80);
            })
            ->AsShared(),
        })
        ->SetSpacing(1.0f)
        ->BuildUI(get_transform());

        Refresh();
    }
}
