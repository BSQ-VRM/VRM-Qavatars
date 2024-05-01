#pragma once

#include "AssetLib/VRMConcepts.hpp"
#include "AssetLib/structure/VRM/VRMmodelContext.hpp"
#include "UnityEngine/GameObject.hpp"

namespace AssetLib::Generators
{
    class SpringBoneGenerator
    {
        public:
        SpringBoneGenerator() = default;
        ~SpringBoneGenerator() = default;

        template<AssetLib::VRMVersion T>
        void Generate(const T& vrm, AssetLib::Structure::VRM::VRMModelContext* modelContext, UnityEngine::GameObject* secondary);
    };
};