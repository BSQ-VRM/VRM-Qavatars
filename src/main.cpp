#include "main.hpp"

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

static CModInfo* modInfo;

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
        co_yield custom_types::Helpers::CoroutineHelper::New(VRMQavatars::ShaderLoader::LoadBund());
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
    getLogger().info("x1");
    SceneManager_Internal_ActiveSceneChanged(prevScene, nextScene);
    getLogger().info("x2");
    VRMQavatars::SceneEventManager::GameSceneChanged(nextScene);
    getLogger().info("finish");
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
    getLogger().info("x1");
    getLogger().info("%d", mainEffect->get_hasPostProcessEffect());
    getLogger().info("x2");
    BloomHook(self, mainEffect);
    VRMQavatars::MaterialTracker::bloomEnabled = mainEffect->get_hasPostProcessEffect();
    VRMQavatars::MaterialTracker::UpdateMaterials();
    getLogger().info("x3");
}

extern "C" void setup(CModInfo* info) {
    info->id        = "vrm-qavatars";
    info->version   = VERSION;
    info->version_long = GIT_COMMIT;

    modInfo = info;

    getGlobalConfig().Init(Configuration::getConfigFilePath(modloader::ModInfo(*modInfo)));
    getLogger().info("hii");
}

extern "C" void load() {
    getLogger().info("load");
    il2cpp_functions::Init();
    getLogger().info("load1");
    mkpath(vrm_path);
    mkpath(avaconfig_path);
    getLogger().info("load2");
    custom_types::Register::AutoRegister(); 
    getLogger().info("load3");
    BSML::Register::RegisterMainMenu<FlowCoordinators::AvatarsFlowCoordinator*>("Avatars", "VRM Custom Avatars");
    getLogger().info("load4");
    INSTALL_HOOK(getLogger(), MainMenuUIHook);
    //INSTALL_HOOK(getLogger(), MainCameraHook);
    //INSTALL_HOOK(getLogger(), SceneManager_Internal_ActiveSceneChanged);
    //INSTALL_HOOK(getLogger(), SaberPatch);
    //INSTALL_HOOK(getLogger(), BloomHook);
    getLogger().info("load5");
}