#pragma once

#include "custom-types/shared/coroutine.hpp"

#include "UnityEngine/AssetBundle.hpp"

namespace VRM
{
    class ShaderLoader
    {
    public:
        static custom_types::Helpers::Coroutine LoadBundleFromFileAsync(std::string_view filePath, UnityEngine::AssetBundle*& out);

        static custom_types::Helpers::Coroutine LoadBundleFromMemoryAsync(ArrayW<uint8_t> bytes, UnityEngine::AssetBundle*& out);

        static custom_types::Helpers::Coroutine LoadAssetFromBundleAsync(UnityEngine::AssetBundle* bundle, std::string_view name, System::Type* type, UnityEngine::Object*& out);
    };
};