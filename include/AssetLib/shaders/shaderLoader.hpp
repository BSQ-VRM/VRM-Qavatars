#pragma once

#include "ShaderSO.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "UnityEngine/AssetBundle.hpp"

namespace VRMQavatars
{
    class ShaderLoader
    {
    public:
        static SafePtrUnity<VRMData::ShaderSO> shaders;

        static custom_types::Helpers::Coroutine LoadBund();

        static custom_types::Helpers::Coroutine LoadBundleFromMemoryAsync(ArrayW<uint8_t> bytes, UnityEngine::AssetBundle*& out);

        static custom_types::Helpers::Coroutine LoadAssetFromBundleAsync(UnityEngine::AssetBundle* bundle, std::string_view name, System::Type* type, UnityEngine::Object*& out);
    };
};