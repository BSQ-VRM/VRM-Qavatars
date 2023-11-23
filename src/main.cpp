#include "main.hpp"

#include <UnityEngine/Camera.hpp>
#include <UnityEngine/SceneManagement/SceneManager.hpp>

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
#include "UnityEngine/AudioListener.hpp"

#include "System/Action_1.hpp"

#include "GlobalNamespace/MainEffectController.hpp"
#include "GlobalNamespace/VisualEffectsController.hpp"
#include "GlobalNamespace/MainCamera.hpp"

#include "AssetLib/shaders/shaderLoader.hpp"
#include "AssetLib/shaders/ShaderSO.hpp"
#include "AssetLib/modelImporter.hpp"

#include "customTypes/TargetManager.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "bsml/shared/BSML/FloatingScreen/FloatingScreen.hpp"

#include "UI/AvatarsFlowCoordinator.hpp"
#include "..\include\UI\components\AvatarListItem.hpp"
#include "..\include\UI\components\AvatarListCell.hpp"

#include "LightManager.hpp"
#include "sceneEventManager.hpp"

#include "config/ConfigManager.hpp"

#include "questui/shared/ArrayUtil.hpp"
#include "AssetLib/structure/VRM/VRMBlendShapeMaster.hpp"
static ModInfo modInfo;

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

    UnityEngine::GameObject::New_ctor("LightManager")->AddComponent<VRMQavatars::LightManager*>();

    getLogger().info("Starting Load!");

    UnityEngine::AssetBundle* ass;
    co_yield coro(VRM::ShaderLoader::LoadBundleFromFileAsync("sdcard/ModData/shaders.sbund", ass));
    if (!ass)
    {
        getLogger().error("Couldn't load bundle from file, dieing...");
        co_return;
    }

    VRMData::ShaderSO* data = nullptr;
    co_yield coro(VRM::ShaderLoader::LoadAssetFromBundleAsync(ass, "Assets/shaders.asset", csTypeOf(VRMData::ShaderSO*), reinterpret_cast<UnityEngine::Object*&>(data)));
    if(data == nullptr)
    {
        getLogger().error("Couldn't load asset...");
        co_return;
    }

    ass->Unload(false);

    AssetLib::ModelImporter::mtoon = data->mToonShader;

    const int tpmask =
           2147483647 &
           ~(1 << 6);

    const int fpmask =
           2147483647 &
           ~(1 << 3);

    auto mainCamera = UnityEngine::GameObject::FindGameObjectWithTag("MainCamera");
    mainCamera->GetComponent<UnityEngine::Camera*>()->set_cullingMask(fpmask);

    auto mirror = UnityEngine::GameObject::Instantiate(data->mirror);

    auto screen = BSML::FloatingScreen::CreateFloatingScreen({32.5f, 54.0f}, true, {0.0f, 1.5f, 2.0f}, UnityEngine::Quaternion::Euler(15.0f, 180.0f, 0.0f), 0.0f, true);
    mirror->get_transform()->SetParent(screen->get_transform(), false);
    UnityEngine::GameObject::DontDestroyOnLoad(screen->get_gameObject());
    mirror->get_transform()->set_localScale({32.0f, 32.0f, 32.0f});
    mirror->get_transform()->set_localPosition({0.0f, 0.0f, 0.05f});

    auto camera = screen->GetComponentInChildren<UnityEngine::Camera*>();

    auto renderTex = camera->get_targetTexture();

    auto parent = camera->get_transform()->get_parent();
    UnityEngine::GameObject::Destroy(camera->get_gameObject());

    auto newCamera = UnityEngine::GameObject::Instantiate(mainCamera, parent, false);
    auto camcomp = newCamera->GetComponent<UnityEngine::Camera*>();

    UnityEngine::GameObject::DestroyImmediate(newCamera->GetComponent<UnityEngine::AudioListener*>());
    UnityEngine::GameObject::DestroyImmediate(newCamera->GetComponent<GlobalNamespace::MainCamera*>());
    UnityEngine::GameObject::DestroyImmediate(newCamera->GetComponent<GlobalNamespace::VisualEffectsController*>());

    camcomp->set_nearClipPlane(1.0f);

    camcomp->set_targetDisplay(0);
    camcomp->set_stereoTargetEye(UnityEngine::StereoTargetEyeMask::None);
    camcomp->set_tag("Untagged");

    camcomp->set_targetTexture(renderTex);

    camcomp->set_cullingMask(tpmask);

    newCamera->get_transform()->set_localPosition({0.0f, 0.0f, -1.0f});
    newCamera->get_transform()->set_localRotation(UnityEngine::Quaternion::Euler(0.0f, 0.0f, 0.0f));

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
    auto& globcon = VRMQavatars::Config::ConfigManager::GetGlobalConfig();
    if(globcon.hasSelected.GetValue())
    {
        const auto path = globcon.selectedFileName.GetValue();
        getLogger().info("%s", (std::string(vrm_path) + "/" + path).c_str());
        if(fileexists(std::string(vrm_path) + "/" + path))
        {
            const auto ctx = AssetLib::ModelImporter::LoadVRM(std::string(vrm_path) + "/" + path, AssetLib::ModelImporter::mtoon.ptr());
            VRMQavatars::AvatarManager::SetContext(ctx);
        }
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

//FP/TP stuff should be in its own mod...
MAKE_HOOK_MATCH(MainCameraHook, &GlobalNamespace::MainCamera::Awake, void, GlobalNamespace::MainCamera* self)
{
    MainCameraHook(self);
    const int fpmask =
           2147483647 &
           ~(1 << 3);
    self->get_camera()->set_cullingMask(fpmask);
}

//I know
//I know
//I know
//Scene manager is bad
//Open a PR (please)
MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged, &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
    SceneManager_Internal_ActiveSceneChanged(prevScene, nextScene);
    VRMQavatars::SceneEventManager::GameSceneChanged(nextScene);
}

extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;

    getGlobalConfig().Init(Configuration::getConfigFilePath(modInfo));
}

extern "C" void load() {
    il2cpp_functions::Init();

    mkpath(vrm_path);
    mkpath(avaconfig_path);

    custom_types::Register::AutoRegister(); 

    BSML::Register::RegisterMainMenu<FlowCoordinators::AvatarsFlowCoordinator*>("Avatars", "VRM Custom Avatars");

    INSTALL_HOOK(getLogger(), MainMenuUIHook);
    INSTALL_HOOK(getLogger(), MainCameraHook);
    INSTALL_HOOK(getLogger(), SceneManager_Internal_ActiveSceneChanged);
}