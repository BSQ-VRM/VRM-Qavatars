#pragma once

#include "AssetLib/VRMConcepts.hpp"

#include "UnityEngine/Material.hpp"
#include "UnityEngine/Texture2D.hpp"

namespace AssetLib::Generators
{
    class MaterialGenerator
    {
        public:
        MaterialGenerator() = default;
        ~MaterialGenerator() = default;

        template<AssetLib::VRMVersion T>
        UnityEngine::Material* Generate(const T& vrm, int index, UnityEngine::Texture2D** textures);
    };
};