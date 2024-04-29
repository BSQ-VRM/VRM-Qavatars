#include "AssetLib/shaders/shaderLoader.hpp"
#include "main.hpp"
#include <string_view>

#include "MirrorManager.hpp"
#include "AssetLib/modelImporter.hpp"
#include "UnityEngine/AssetBundleCreateRequest.hpp"
#include "UnityEngine/AssetBundleRequest.hpp"

#include "assets.hpp"

namespace VRMQavatars
{
    #define coro(...) custom_types::Helpers::CoroutineHelper::New(__VA_ARGS__)

    SafePtrUnity<VRMData::ShaderSO> ShaderLoader::shaders;

    custom_types::Helpers::Coroutine ShaderLoader::LoadBund()
    {
        VRMLogger.info("Loading shaders...");
        UnityEngine::AssetBundle* ass;
        co_yield coro(ShaderLoader::LoadBundleFromMemoryAsync(Assets::shaders_sbund, ass));
        if (!ass)
        {
            VRMLogger.error("Couldn't load bundle from file, dieing...");
            co_return;
        }
        VRMLogger.info("Loaded Bundle");

        VRMData::ShaderSO* data = nullptr;
        co_yield coro(ShaderLoader::LoadAssetFromBundleAsync(ass, "Assets/shaders.asset", csTypeOf(VRMData::ShaderSO*), reinterpret_cast<UnityEngine::Object*&>(data)));
        if(data == nullptr)
        {
            VRMLogger.error("Couldn't load asset...");
            co_return;
        }
        ass->Unload(false);
        VRMLogger.info("Loaded asset");

        AssetLib::ModelImporter::mtoon = data->mToonShader;
        MirrorManager::mirrorShader = data->mirrorShader;
        shaders = data;
        VRMLogger.info("Finished Loading assets");
        co_return;
    }

    custom_types::Helpers::Coroutine ShaderLoader::LoadBundleFromMemoryAsync(ArrayW<uint8_t> bytes, UnityEngine::AssetBundle*& out)
    {
        using AssetBundle_LoadFromMemoryAsync = function_ptr_t<UnityEngine::AssetBundleCreateRequest*, ArrayW<uint8_t>, int>;
        static auto assetBundle_LoadFromMemoryAsync = reinterpret_cast<AssetBundle_LoadFromMemoryAsync>(il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadFromMemoryAsync_Internal"));

        auto req = assetBundle_LoadFromMemoryAsync(bytes, 0);
        req->set_allowSceneActivation(true);
        while (!req->get_isDone())
            co_yield nullptr;

        out = req->get_assetBundle();
        UnityEngine::Object::DontDestroyOnLoad(out);
        co_return;
    }

    custom_types::Helpers::Coroutine ShaderLoader::LoadAssetFromBundleAsync(UnityEngine::AssetBundle* bundle, std::string_view name, System::Type* type, UnityEngine::Object*& out)
    {
        auto req = bundle->LoadAssetAsync(name, type);
        req->set_allowSceneActivation(true);
        while (!req->get_isDone())
            co_yield nullptr;

        out = req->get_asset();
        UnityEngine::Object::DontDestroyOnLoad(out);
        co_return;
    }
}