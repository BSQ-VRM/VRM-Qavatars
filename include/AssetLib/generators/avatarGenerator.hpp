#pragma once

#include "AssetLib/VRMConcepts.hpp"

#include "UnityEngine/Avatar.hpp"
#include "UnityEngine/GameObject.hpp"

#include "AssetLib/structure/node.hpp"

namespace AssetLib::Generators
{
    class AvatarGenerator
    {
        public:
        AvatarGenerator() = default;
        ~AvatarGenerator() = default;

        template<AssetLib::VRMVersion T>
        UnityEngine::Avatar* Generate(const T& vrm, std::vector<AssetLib::Structure::Node*> bones, UnityEngine::GameObject* root);
    };
};