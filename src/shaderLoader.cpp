#include "shaderLoader.hpp"

#include "UnityEngine/AssetBundleCreateRequest.hpp"
#include "UnityEngine/AssetBundleRequest.hpp"

custom_types::Helpers::Coroutine VRM::ShaderLoader::LoadBundleFromMemoryAsync(ArrayW<uint8_t> bytes, UnityEngine::AssetBundle*& out)
{
    using AssetBundle_LoadFromMemoryAsync = function_ptr_t<UnityEngine::AssetBundleCreateRequest*, ArrayW<uint8_t>, int>;
    static AssetBundle_LoadFromMemoryAsync assetBundle_LoadFromMemoryAsync = reinterpret_cast<AssetBundle_LoadFromMemoryAsync>(il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadFromMemoryAsync_Internal"));

    auto req = assetBundle_LoadFromMemoryAsync(bytes, 0);
    req->set_allowSceneActivation(true);
    while (!req->get_isDone())
        co_yield nullptr;

    out = req->get_assetBundle();
    co_return;
}

custom_types::Helpers::Coroutine VRM::ShaderLoader::LoadAssetFromBundleAsync(UnityEngine::AssetBundle* bundle, std::string_view name, System::Type* type, UnityEngine::Object*& out)
{
    auto req = bundle->LoadAssetAsync(name, type);
    req->set_allowSceneActivation(true);
    while (!req->get_isDone())
        co_yield nullptr;

    out = req->get_asset();

    co_return;
}