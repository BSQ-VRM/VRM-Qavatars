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
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Resources.hpp"

#include "GlobalNamespace/BloomPrePass.hpp"
#include "GlobalNamespace/ImageEffectController.hpp"

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

    auto light = UnityEngine::GameObject::New_ctor()->AddComponent<UnityEngine::Light*>();
    light->set_intensity(1.0f);
    static auto setType = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, UnityEngine::LightType>("UnityEngine.Light::set_type");
    setType(light, UnityEngine::LightType::Directional);

    UnityEngine::GameObject::DontDestroyOnLoad(light->get_gameObject());

    auto mirror = UnityEngine::GameObject::Instantiate(data->mirror);

    auto screen = BSML::FloatingScreen::CreateFloatingScreen({25.0f, 44.44f}, true, {0.0f, 1.5f, 2.0f}, UnityEngine::Quaternion::Euler(15.0f, 180.0f, 0.0f), 0.0f, true);
    mirror->get_transform()->SetParent(screen->get_transform(), false);
    UnityEngine::GameObject::DontDestroyOnLoad(screen->get_gameObject());
    mirror->get_transform()->set_localScale({25.0f, 25.0f, 25.0f});
    mirror->get_transform()->set_localPosition({0.0f, 0.0f, 0.05f});

    auto camera = screen->GetComponentInChildren<UnityEngine::Camera*>();

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

    mkpath(vrm_path);

    custom_types::Register::AutoRegister(); 

    BSML::Register::RegisterMainMenu<VRMQavatars::UI::FlowCoordinators::AvatarsFlowCoordinator*>("Avatars", "VRM Custom Avatars");

    INSTALL_HOOK(getLogger(), MainMenuUIHook);
}