#include "main.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/register.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"

#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/AssetBundleCreateRequest.hpp"
#include "UnityEngine/AssetBundleRequest.hpp"
#include "UnityEngine/Light.hpp"
#include "UnityEngine/LightType.hpp"

#include "AssetLib/shaders/shaderLoader.hpp"
#include "AssetLib/shaders/ShaderSO.hpp"
#include "AssetLib/modelImporter.hpp"

#include "customTypes/TargetManager.hpp"

#include "bsml/shared/BSML.hpp"
#include "UI/AvatarsFlowCoordinator.hpp"
#include "UI/components/AvatarListTableData.hpp"
#include "UI/components/AvatarListTableCell.hpp"

static ModInfo modInfo;

Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

#define coro(...) custom_types::Helpers::CoroutineHelper::New(__VA_ARGS__)

custom_types::Helpers::Coroutine Setup()
{
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

    auto light = UnityEngine::GameObject::New_ctor()->AddComponent<UnityEngine::Light*>();
    light->set_intensity(1000.0f);
    static auto setType = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, UnityEngine::LightType>("UnityEngine.Light::set_type");
    setType(light, UnityEngine::LightType::Directional);

    UnityEngine::GameObject::DontDestroyOnLoad(light->get_gameObject());
    
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

    mkpath(vrm_path);

    custom_types::Register::AutoRegister(); 

    BSML::Register::RegisterMainMenu<VRMQavatars::UI::FlowCoordinators::AvatarsFlowCoordinator*>("Avatars", "VRM Custom Avatars");

    INSTALL_HOOK(getLogger(), MainMenuUIHook);
}