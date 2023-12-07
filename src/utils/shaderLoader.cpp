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

    custom_types::Helpers::Coroutine ShaderLoader::LoadBund()
    {
        UnityEngine::AssetBundle* ass;
        co_yield coro(ShaderLoader::LoadBundleFromFileAsync("sdcard/ModData/shaders.sbund", ass));
        if (!ass)
        {
            getLogger().error("Couldn't load bundle from file, dieing...");
            co_return;
        }
        VRMData::ShaderSO* data = nullptr;
        co_yield coro(ShaderLoader::LoadAssetFromBundleAsync(ass, "Assets/shaders.asset", csTypeOf(VRMData::ShaderSO*), reinterpret_cast<UnityEngine::Object*&>(data)));
        if(data == nullptr)
        {
            getLogger().error("Couldn't load asset...");
            co_return;
        }
        ass->Unload(false);
        AssetLib::ModelImporter::mtoon = data->mToonShader;
        MirrorManager::mirrorShader = data->mirrorShader;

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