#include "main.hpp"

#include <UnityEngine/Camera.hpp>

#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/register.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"

#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/AssetBundleCreateRequest.hpp"
#include "UnityEngine/AssetBundleRequest.hpp"
#include "UnityEngine/Light.hpp"
#include "UnityEngine/LightType.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/StereoTargetEyeMask.hpp"

#include "System/Action_1.hpp"

#include "GlobalNamespace/MainEffectController.hpp"

#include "AssetLib/shaders/shaderLoader.hpp"
#include "AssetLib/shaders/ShaderSO.hpp"
#include "AssetLib/modelImporter.hpp"

#include "customTypes/TargetManager.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "bsml/shared/BSML/FloatingScreen/FloatingScreen.hpp"

#include "UI/AvatarsFlowCoordinator.hpp"
#include "UI/components/AvatarListTableData.hpp"
#include "UI/components/AvatarListTableCell.hpp"

#include "LightManager.hpp"

#include "config/ConfigManager.hpp"

#include "questui/shared/ArrayUtil.hpp"
static ModInfo modInfo;

Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

UnityEngine::Sprite* GetBGSprite(std::string str)
{
    return QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Sprite*>(),
                                     [str](UnityEngine::Sprite* x) {
                                         return x->get_name() == str;
                                     });
}

UnityEngine::Material* GetBGMat(std::string str)
{
    return QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>(),
                                     [str](UnityEngine::Material* x) {
                                         return x->get_name() == str;
                                     });
}

UnityEngine::Texture2D* GetRTPixels(UnityEngine::RenderTexture* rt)
{
    // Remember currently active render texture
    UnityEngine::RenderTexture* currentActiveRT = UnityEngine::RenderTexture::get_active();

    // Set the supplied RenderTexture as the active one
    UnityEngine::RenderTexture::set_active(rt);

    // Create a new Texture2D and read the RenderTexture image into it
    UnityEngine::Texture2D* tex = UnityEngine::Texture2D::New_ctor(rt->get_width(), rt->get_height());
    tex->ReadPixels(UnityEngine::Rect(0, 0, tex->get_width(), tex->get_height()), 0, 0);
    tex->Apply();

    // Restore previously active render texture
    UnityEngine::RenderTexture::set_active(currentActiveRT);
    return tex;
}

#define coro(...) custom_types::Helpers::CoroutineHelper::New(__VA_ARGS__)

custom_types::Helpers::Coroutine Setup() {
    getLogger().info("Starting Load!");

    UnityEngine::AssetBundle* ass;
    co_yield coro(VRM::ShaderLoader::LoadBundleFromFileAsync("sdcard/ModData/shaders.sbund", ass));
    if (!ass)
    {
        getLogger().error("Couldn't load bundle from file, dieing...");
        co_return;
    }

    VRMData::ShaderSO* data = nullptr;
    co_yield coro(VRM::ShaderLoader::LoadAssetFromBundleAsync(ass, "Assets/shaders.asset", reinterpret_cast<System::Type*>(csTypeOf(VRMData::ShaderSO*)), reinterpret_cast<UnityEngine::Object*&>(data)));
    if(data == nullptr)
    {
        getLogger().error("Couldn't load asset...");
        co_return;
    }

    AssetLib::ModelImporter::mtoon = data->mToonShader;

    VRMQavatars::LightManager::Init();

    auto mirror = UnityEngine::GameObject::Instantiate(data->mirror);

    auto screen = BSML::FloatingScreen::CreateFloatingScreen({32.5f, 54.0f}, true, {0.0f, 1.5f, 2.0f}, UnityEngine::Quaternion::Euler(15.0f, 180.0f, 0.0f), 0.0f, true);
    mirror->get_transform()->SetParent(screen->get_transform(), false);
    UnityEngine::GameObject::DontDestroyOnLoad(screen->get_gameObject());
    mirror->get_transform()->set_localScale({32.0f, 32.0f, 32.0f});
    mirror->get_transform()->set_localPosition({0.0f, 0.0f, 0.05f});

    /*auto camera = screen->GetComponentInChildren<UnityEngine::Camera*>();
    auto parent = camera->get_transform()->get_parent();
    UnityEngine::GameObject::Destroy(camera->get_gameObject());

    auto mainCamera = UnityEngine::GameObject::FindGameObjectWithTag("MainCamera");
    auto newCamera = UnityEngine::GameObject::Instantiate(mainCamera, parent, false);
    auto camcomp = newCamera->GetComponent<UnityEngine::Camera*>();
    camcomp->set_targetDisplay(0);
    camcomp->set_stereoTargetEye(UnityEngine::StereoTargetEyeMask::None);

    using Action = System::Action_1<UnityEngine::RenderTexture*>*;

    auto effect = newCamera->GetComponentInChildren<GlobalNamespace::MainEffectController*>();
    auto material = screen->GetComponentInChildren<UnityEngine::Material*>();
    effect->add_afterImageEffectEvent(custom_types::MakeDelegate<Action>(classof(Action), static_cast<std::function<void(UnityEngine::RenderTexture*)>>([material](UnityEngine::RenderTexture* texture){
        material->set_mainTexture(GetRTPixels(texture));
    })));*/

    auto getBgSprite = GetBGSprite("RoundRect10BorderFade");

    for(auto x : screen->GetComponentsInChildren<HMUI::ImageView*>()) {
        if(!x)
            continue;
        x->skew = 0.0f;
        x->set_overrideSprite(nullptr);
        x->set_sprite(getBgSprite);
        x->set_material(GetBGMat("UINoGlow"));
        x->set_color({1.0f, 1.0f, 1.0f, 1.0f});
    }

    co_return;
}
 
MAKE_HOOK_MATCH(MainMenuUIHook, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuUIHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if(firstActivation)
    {
        self->StartCoroutine(coro(Setup()));
    }
}

extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
    
    getConfig().Load();
}

extern "C" void load() {
    il2cpp_functions::Init();

    getGlobalConfig().Init(Configuration::getConfigFilePath(modInfo));

    mkpath(vrm_path);
    mkpath(avaconfig_path);

    custom_types::Register::AutoRegister(); 

    BSML::Register::RegisterMainMenu<VRMQavatars::UI::FlowCoordinators::AvatarsFlowCoordinator*>("Avatars", "VRM Custom Avatars");

    INSTALL_HOOK(getLogger(), MainMenuUIHook);
}