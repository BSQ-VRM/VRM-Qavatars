#pragma once

#include "custom-types/shared/coroutine.hpp"

#include "UnityEngine/AssetBundle.hpp"

namespace VRM
{
    class ShaderLoader
    {
    public:
        custom_types::Helpers::Coroutine LoadBundleFromMemoryAsync(ArrayW<uint8_t> bytes, UnityEngine::AssetBundle*& out);

        custom_types::Helpers::Coroutine LoadAssetFromBundleAsync(UnityEngine::AssetBundle* bundle, std::string_view name, System::Type* type, UnityEngine::Object*& out);
    };
};