#pragma once

#include "AssetLib/VRMConcepts.hpp"

// Include System/String.hpp first to break circular include: HumanBone_impl -> IEquatable_1_def -> typedefs.h -> String.hpp -> IEquatable_1 (not yet declared)
#include "System/String.hpp"
// Ensure HumanBone/SkeletonBone are fully defined before Avatar.hpp triggers HumanDescription_impl
#include "UnityEngine/zzzz__HumanBone_impl.hpp"
#include "UnityEngine/zzzz__SkeletonBone_impl.hpp"
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