#include "AssetLib/shaders/shaderLoader.hpp"
#include "main.hpp"
#include <string_view>

#include "MirrorManager.hpp"
#include "AssetLib/modelImporter.hpp"
#include "UnityEngine/AssetBundleCreateRequest.hpp"
#include "UnityEngine/AssetBundleRequest.hpp"

namespace VRMQavatars
{
    #define coro(...) custom_types::Helpers::CoroutineHelper::New(__VA_ARGS__)

    SafePtrUnity<VRMData::ShaderSO> ShaderLoader::shaders;

    custom_types::Helpers::Coroutine ShaderLoader::LoadBund()
    {
        getLogger().info("l1");
        UnityEngine::AssetBundle* ass;
        getLogger().info("l2");
        co_yield coro(ShaderLoader::LoadBundleFromFileAsync("sdcard/ModData/shaders.sbund", ass));
        getLogger().info("l3");
        if (!ass)
        {
            getLogger().error("Couldn't load bundle from file, dieing...");
            co_return;
        }
        getLogger().info("l4");
        VRMData::ShaderSO* data = nullptr;
        getLogger().info("l5");
        co_yield coro(ShaderLoader::LoadAssetFromBundleAsync(ass, "Assets/shaders.asset", csTypeOf(VRMData::ShaderSO*), reinterpret_cast<UnityEngine::Object*&>(data)));
        getLogger().info("l6");
        if(data == nullptr)
        {
            getLogger().error("Couldn't load asset...");
            co_return;
        }
        getLogger().info("l7");
        ass->Unload(false);
        getLogger().info("l8");
        AssetLib::ModelImporter::mtoon = data->mToonShader;
        MirrorManager::mirrorShader = data->mirrorShader;
        getLogger().info("l9");
        shaders = data;
        getLogger().info("l10");
        co_return;
    }

    custom_types::Helpers::Coroutine ShaderLoader::LoadBundleFromFileAsync(std::string_view filePath, UnityEngine::AssetBundle*& out)
    {
        if (!fileexists(filePath))
        {
            getLogger().error("File %s did not exist", filePath.data());
            out = nullptr;
            co_return;
        }

        auto req = UnityEngine::AssetBundle::LoadFromFileAsync(filePath);
        req->set_allowSceneActivation(true);
        while (!req->get_isDone())
            co_yield nullptr;

        out = req->get_assetBundle();
        co_return;
    }

    custom_types::Helpers::Coroutine ShaderLoader::LoadAssetFromBundleAsync(UnityEngine::AssetBundle* bundle, std::string_view name, System::Type* type, UnityEngine::Object*& out)
    {
        auto req = bundle->LoadAssetAsync(name, type);
        req->set_allowSceneActivation(true);
        while (!req->get_isDone())
            co_yield nullptr;

        out = req->get_asset();

        co_return;
    }
}