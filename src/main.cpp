#include "main.hpp"

#include "scotland2/shared/loader.hpp"

#include <conditional-dependencies/shared/main.hpp>
#include <GlobalNamespace/Saber.hpp>
#include <UnityEngine/Resources.hpp>

#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/register.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/MainCamera.hpp"
#include "GlobalNamespace/MainEffectContainerSO.hpp"
#include "GlobalNamespace/MainEffectSO.hpp"

#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/QualitySettings.hpp"
#include "UnityEngine/AssetBundleRequest.hpp"
#include "UnityEngine/AsyncOperation.hpp"
#include "UnityEngine/AssetBundleCreateRequest.hpp"

#include "AssetLib/shaders/shaderLoader.hpp"
#include "AssetLib/shaders/ShaderSO.hpp"
#include "AssetLib/modelImporter.hpp"

#include "UI/AvatarsFlowCoordinator.hpp"

#include "LightManager.hpp"
#include "SceneEventManager.hpp"
#include "GroundOffsetManager.hpp"
#include "MaterialTracker.hpp"
#include "MirrorManager.hpp"

#include "config/ConfigManager.hpp"

#include "VMC/VMCClient.hpp"
#include "VMC/VMCServer.hpp"

modloader::ModInfo modInfo{MOD_ID, VERSION, GIT_COMMIT};

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

custom_types::Helpers::Coroutine Setup() {
    getLogger().info("x1");
    if(!VRMQavatars::ShaderLoader::shaders)
    {
        getLogger().info("x2");
        //Womp womp
        if(UnityEngine::Resources::FindObjectsOfTypeAll<VRMQavatars::LightManager*>().size() == 0)
        {
            UnityEngine::GameObject::New_ctor("LightManager")->AddComponent<VRMQavatars::LightManager*>();
        }
        getLogger().info("x3");
        getLogger().info("l1");
        UnityEngine::AssetBundle* ass;
        getLogger().info("l2");
        auto req = UnityEngine::AssetBundle::LoadFromFileAsync("sdcard/ModData/shaders.sbund");
        getLogger().info("l3");
        req->set_allowSceneActivation(true);
        while (!req->get_isDone())
            co_yield nullptr;
        getLogger().info("l4");
        ass = req->get_assetBundle();
        getLogger().info("l5");
        if (!ass)
        {
            getLogger().error("Couldn't load bundle from file, dieing...");
            co_return;
        }
        getLogger().info("l6");
        VRMData::ShaderSO* data = nullptr;
        getLogger().info("l7");
        auto req2 = ass->LoadAssetAsync("Assets/shaders.asset", csTypeOf(VRMData::ShaderSO*));
        getLogger().info("l8");
        req2->set_allowSceneActivation(true);
        getLogger().info("l9");
        while (!req2->get_isDone())
            co_yield nullptr;
        getLogger().info("l10");
        data = req2->get_asset().try_cast<VRMData::ShaderSO>().value_or(nullptr);
        getLogger().info("l11");
        if(data == nullptr)
        {
            getLogger().error("Couldn't load asset...");
            co_return;
        }
        getLogger().info("l12");
        ass->Unload(false);
        getLogger().info("l13");
        AssetLib::ModelImporter::mtoon = data->mToonShader;
        VRMQavatars::MirrorManager::mirrorShader = data->mirrorShader;
        getLogger().info("l14");
        VRMQavatars::ShaderLoader::shaders = data;
        getLogger().info("l15");
        getLogger().info("x4");
    }
    getLogger().info("x5");
    if(VRMQavatars::AvatarManager::currentContext == nullptr)
    {
        getLogger().info("x6");
        VRMQavatars::AvatarManager::StartupLoad();
    }
    getLogger().info("x7");

    VRMQavatars::VMC::VMCClient::InitClient();
    VRMQavatars::VMC::VMCServer::InitServer();
    getLogger().info("x8");
    VRMQavatars::GroundOffsetManager::Init();
    getLogger().info("x9");
    VRMQavatars::MirrorManager::CreateMainMirror();
    getLogger().info("x10");

    //AssetLib::ModelImporter::LoadVRM("sdcard/ModData/com.beatgames.beatsaber/Mods/VRMQavatars/Avatars/sampBFixed.vrm", AssetLib::ModelImporter::mtoon.ptr());
    co_return;
}
 
MAKE_HOOK_MATCH(MainMenuUIHook, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuUIHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if(firstActivation)
    {
        self->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(Setup()));
    }
}

// FP/TP stuff should be in its own mod...
MAKE_HOOK_MATCH(MainCameraHook, &GlobalNamespace::MainCamera::Awake, void, GlobalNamespace::MainCamera* self)
{
    MainCameraHook(self);
    constexpr int fpmask =
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

MAKE_HOOK_MATCH(SaberPatch, &GlobalNamespace::Saber::ManualUpdate, void, GlobalNamespace::Saber* self) {
    SaberPatch(self);
    const static auto replay = CondDeps::Find<bool>("replay", "IsInReplay");
    if(replay.has_value() && replay.value()())
    {
        if(VRMQavatars::AvatarManager::currentContext != nullptr)
        {
            const auto targetManager = VRMQavatars::AvatarManager::currentContext->rootGameObject->GetComponent<VRMQavatars::TargetManager*>();
            const auto type = self->get_saberType();
            if(type == GlobalNamespace::SaberType::SaberA)
            {
                targetManager->replayLeftSaberPos = self->get_transform()->get_position();
                targetManager->replayLeftSaberRot = self->get_transform()->get_rotation();
            }
            if(type == GlobalNamespace::SaberType::SaberB)
            {
                targetManager->replayRightSaberPos = self->get_transform()->get_position();
                targetManager->replayRightSaberRot = self->get_transform()->get_rotation();
            }
        }
    }
}

MAKE_HOOK_MATCH(BloomHook, &GlobalNamespace::MainEffectContainerSO::Init, void, GlobalNamespace::MainEffectContainerSO* self, GlobalNamespace::MainEffectSO* mainEffect) {
    BloomHook(self, mainEffect);
    VRMQavatars::MaterialTracker::bloomEnabled = mainEffect->get_hasPostProcessEffect();
    VRMQavatars::MaterialTracker::UpdateMaterials();
}

extern "C" void setup(CModInfo* info) {
    info->id = modInfo.id.c_str();
    info->version = modInfo.version.c_str();
    info->version_long = modInfo.versionLong;

    getGlobalConfig().Init(Configuration::getConfigFilePath(modInfo));
    getLogger().info("hii");
}

extern "C" void late_load() {
    getLogger().info("load");
    il2cpp_functions::Init();
    getLogger().info("load1");
    mkpath(vrm_path);
    mkpath(avaconfig_path);
    getLogger().info("load2");
    custom_types::Register::AutoRegister(); 
    getLogger().info("load3");
    getLogger().info("load4");
    INSTALL_HOOK(getLogger(), MainMenuUIHook);
    INSTALL_HOOK(getLogger(), MainCameraHook);
    INSTALL_HOOK(getLogger(), SceneManager_Internal_ActiveSceneChanged);
    INSTALL_HOOK(getLogger(), SaberPatch);
    INSTALL_HOOK(getLogger(), BloomHook);
    BSML::Register::RegisterMainMenu<FlowCoordinators::AvatarsFlowCoordinator*>("Avatars", "VRM Custom Avatars");
    getLogger().info("load5");
}