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
#include "UnityEngine/QualitySettings.hpp"

#include "AssetLib/shaders/shaderLoader.hpp"
#include "AssetLib/shaders/ShaderSO.hpp"
#include "AssetLib/modelImporter.hpp"

#include "UI/AvatarsFlowCoordinator.hpp"

#include "AvatarManager.hpp"
#include "LightManager.hpp"
#include "SceneEventManager.hpp"
#include "GroundOffsetManager.hpp"
#include "MaterialTracker.hpp"
#include "MirrorManager.hpp"

#include "config/ConfigManager.hpp"

#include "VMC/VMCClient.hpp"
#include "VMC/VMCServer.hpp"

#include "customTypes/FinalIK/VRIK.hpp"
#include "customTypes/Installers/MenuInstaller.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"
#include "lapiz/shared/zenject/Location.hpp"

modloader::ModInfo modInfo{MOD_ID, VERSION, GIT_COMMIT};

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
    VRMLogger.info("hii");
}

extern "C" void late_load() {
    il2cpp_functions::Init();

    mkpath(vrm_path);
    mkpath(avaconfig_path);

    custom_types::Register::AutoRegister(); 

    INSTALL_HOOK(VRMLogger, MainCameraHook);
    INSTALL_HOOK(VRMLogger, SceneManager_Internal_ActiveSceneChanged);
    INSTALL_HOOK(VRMLogger, SaberPatch);
    INSTALL_HOOK(VRMLogger, BloomHook);

    auto zenjeqtor = Lapiz::Zenject::Zenjector::Get();

    zenjeqtor->Install<VRMQavatars::MenuInstaller*>(Lapiz::Zenject::Location::Menu);

    BSML::Register::RegisterMainMenu<FlowCoordinators::AvatarsFlowCoordinator*>("Avatars", "VRM Custom Avatars");
}