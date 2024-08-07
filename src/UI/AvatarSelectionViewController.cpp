#include "UI/AvatarSelectionViewController.hpp"

#include <string_view>
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "UnityEngine/TextureFormat.hpp"

#include "AvatarManager.hpp"
#include "assets.hpp"
#include "CalibrationHelper.hpp"
#include "TPoseHelper.hpp"
#include "AssetLib/modelImporter.hpp"
#include "AssetLib/gLTFImageReader.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/XUI/Templates.hpp"
#include "config/ConfigManager.hpp"
#include "UI/components/AvatarListCell.hpp"
#include "UI/components/AvatarListItem.hpp"

#include "utils/FileUtils.hpp"

#include "bsml/shared/BSML/MainThreadScheduler.hpp"

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

    VRMDescriptor GetNoneDescriptor()
    {
        VRMDescriptor descriptor;
        descriptor.name = "None";
        descriptor.author = "No avatar";

        UnityEngine::Texture2D* texture = UnityEngine::Texture2D::New_ctor(0, 0, UnityEngine::TextureFormat::RGBA32, false, false);
        UnityEngine::ImageConversion::LoadImage(texture, Assets::none_png, false);

        descriptor.thumbnail = texture;

        descriptor.filePath = "";

        return descriptor;
    }

    std::ifstream::pos_type filesize(const char* filename)
    {
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        return in.tellg();
    }

    VRMDescriptor LoadVRMDescriptor(const std::string& path) {
        VRMDescriptor descriptor;

        if(filesize((vrm_path + std::string("/") + path).c_str()) < 1)
        {
            VRMLogger.info("die");
            descriptor.valid = false;
            return descriptor;
        }

        //Load in binary to parse out VRM data
        std::ifstream binFile(vrm_path + std::string("/") + path, std::ios::binary);
        binFile.seekg(12); //Skip past the 12 byte header, to the json header
        uint32_t jsonLength;
        binFile.read(reinterpret_cast<char*>(&jsonLength), sizeof(uint32_t)); //Read the length of the json file from it's header
        VRMLogger.info("{}", jsonLength);
        std::string jsonStr;
        jsonStr.resize(jsonLength);
        binFile.seekg(20); // Skip the rest of the JSON header to the start of the string
        binFile.read(jsonStr.data(), jsonLength); // Read out the json string

        nlohmann::json doc;
        try
        {
            doc = nlohmann::json::parse(jsonStr);
        }
        catch (nlohmann::json::parse_error& ex)
        {
            VRMLogger.error("parse error at byte {}", ex.byte);
            descriptor.valid = false;
            return descriptor;
        }
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

        descriptor.thumbnail = AssetLib::gLTFImageReader::ReadImageIndex(jsonLength, binFile, descriptor.vrm0.has_value() ? descriptor.vrm0.value().meta.texture : descriptor.vrm1.value().meta.thumbnailImage);
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

            avatarSet.push_back(std::make_shared<Components::AvatarListItem>(GetNoneDescriptor()));
            for(auto& vrm : vrms)
            {
                auto descriptor = LoadVRMDescriptor(vrm);
                if(descriptor.valid)
                {
                    avatarSet.push_back(std::make_shared<Components::AvatarListItem>(descriptor));
                }
            }
            avatarList->SetListItems(avatarSet);
            auto& globcon = Config::ConfigManager::GetGlobalConfig();
            if(globcon.hasSelected.GetValue())
            {
                auto pred = [&globcon](const std::shared_ptr<CP_SDK::UI::Data::IListItem>& ptr) { return reinterpret_cast<const Components::AvatarListItem*>(ptr.get())->descriptor.filePath == globcon.selectedFileName.GetValue(); };
                const auto it = std::find_if(std::begin(avatarSet),
                                             std::end(avatarSet),
                                             pred);
                const auto itemPos = std::distance(std::begin(avatarSet), it);
                const auto item = *reinterpret_cast<const std::shared_ptr<Components::AvatarListItem>*>(&avatarSet[itemPos]);
                avatarList->SetSelectedListItem(item->shared_from_this(), false);
            }
        }
    }

    void AvatarSelectionViewController::OnListSelect(const Components::AvatarListItem::Ptr& p_ListItem)
    {
        auto& globcon = Config::ConfigManager::GetGlobalConfig();
        const auto item = *reinterpret_cast<const std::shared_ptr<Components::AvatarListItem>*>(&p_ListItem);
        if (item)
        {
            Config::ConfigManager::ResetAvatarConfig(false);
            const auto path = item->descriptor.filePath;
            if(!item->descriptor.valid) return;
            if(path == "")
            {
                AvatarManager::SetContext(nullptr);
                globcon.hasSelected.SetValue(false);
                globcon.selectedFileName.SetValue(path);
                return;
            }
            if(fileexists(std::string(vrm_path) + "/" + path))
            {
                std::shared_future<AssetLib::Structure::VRM::VRMModelContext*> future = AssetLib::ModelImporter::LoadVRM(std::string(vrm_path) + "/" + path);
                BSML::MainThreadScheduler::AwaitFuture(future, [path, item, future, this](){
                    AssetLib::Structure::VRM::VRMModelContext* ctx = future.get();
                    AvatarManager::SetContext(ctx);

                    auto& globcon = Config::ConfigManager::GetGlobalConfig();

                    globcon.hasSelected.SetValue(true);
                    globcon.selectedFileName.SetValue(path);

                    auto& avacon = Config::ConfigManager::GetAvatarConfig();
                    if(!avacon.HasAgreedToTerms.GetValue())
                    {
                        agreementModal->SetInfo(item->descriptor);
                        ShowModal(agreementModal.Ptr());
                    }
                    if(avacon.HasCalibrated.GetValue())
                    {
                        CalibrationHelper::Calibrate(avacon.CalibratedScale.GetValue());
                    }
                });
            }
        }
    }

    void AvatarSelectionViewController::Calibrate()
    {
        if(AvatarManager::currentContext == nullptr) return;
        this->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(CalibrationHelper::StartCalibrationProc()));
    }

    void AvatarSelectionViewController::DidActivate()
    {
        agreementModal = CreateModal<Modals::AvatarSelectionModal>();

        CP_SDK::XUI::Templates::FullRectLayoutMainView({
            CP_SDK::XUI::XUIVLayout::Make({
                CP_SDK::XUI::XUIHLayout::Make({
                    CP_SDK::XUI::XUISecondaryButton::Make(u"Refresh")
                        ->OnClick({this, &AvatarSelectionViewController::Refresh})
                        ->AsShared(),
                    CP_SDK::XUI::XUIPrimaryButton::Make(u"Recalibrate")
                        ->OnClick({this, &AvatarSelectionViewController::Calibrate})
                        ->SetInteractable(AvatarManager::currentContext != nullptr)
                        ->Bind(&RecalibrateButton)
                        ->AsShared(),
                })
                ->SetSpacing(8.0f)
                ->AsShared(),
                CP_SDK::XUI::XUIHLayout::Make({
                    CP_SDK::XUI::XUIVVList::Make()
                        ->SetListCellPrefab(CP_SDK::UI::Data::ListCellPrefabs<Components::AvatarListCell>::Get())
                        ->OnListItemSelected({this, &AvatarSelectionViewController::OnListSelect})
                        ->Bind(&avatarList)
                        ->AsShared()
                })
                ->SetHeight(58)
                ->SetSpacing(0)
                ->SetPadding(0)
                ->OnReady([](CP_SDK::UI::Components::CHLayout* x) -> void {
                    x->CSizeFitter()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
                    x->CSizeFitter()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::Unconstrained);
                    x->HOrVLayoutGroup()->set_childForceExpandWidth(true);
                    x->HOrVLayoutGroup()->set_childForceExpandHeight(true);
                    x->LElement()->set_preferredWidth(80);
                })
                ->AsShared(),
            })
            ->SetSpacing(3.0f)
            ->AsShared()
        })
        ->BuildUI(get_transform());

        Refresh();

        AvatarManager::OnLoad += [this]
        {
            RecalibrateButton->SetInteractable(AvatarManager::currentContext != nullptr);
        };
    }
}
