#include "main.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/AssetBundleCreateRequest.hpp"
#include "UnityEngine/AssetBundleRequest.hpp"
#include "AssetLib/shaders/shaderLoader.hpp"
#include "AssetLib/shaders/ShaderSO.hpp"

#include "AssetLib/modelImporter.hpp"


static ModInfo modInfo;

Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

void logTransform(UnityEngine::Transform* trans, int depth = 1)
{
    getLogger().info("%s%s", std::string(depth, '-').c_str(), static_cast<std::string>(trans->get_gameObject()->get_name()).c_str());
    for (size_t i = 0; i < trans->get_childCount(); i++)
    {
        logTransform(trans->GetChild(i), depth + 1);
    }
}

#define coro(...) custom_types::Helpers::CoroutineHelper::New(__VA_ARGS__)

custom_types::Helpers::Coroutine LoadAvatar()
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

    auto ctx = AssetLib::ModelImporter::LoadVRM("sdcard/ModData/ava.vrm", data->mToonShader);
    
    logTransform(ctx->rootNode->gameObject->get_transform());

    //co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(2.0f));

    //ctx->armature.value().bones[18]->gameObject->get_transform()->Translate(UnityEngine::Vector3(0.0f, 1.0f, 0.0f));
    
    co_return;
}
 
MAKE_HOOK_MATCH(MainMenuUIHook, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuUIHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    self->StartCoroutine(coro(LoadAvatar()));
}

extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
    
    getConfig().Load();
}

extern "C" void load() {
    il2cpp_functions::Init();

    INSTALL_HOOK(getLogger(), MainMenuUIHook);
}