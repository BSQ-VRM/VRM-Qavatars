#include "main.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"

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

    auto ctx = AssetLib::ModelImporter::Load("sdcard/ModData/ava.vrm");
    
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