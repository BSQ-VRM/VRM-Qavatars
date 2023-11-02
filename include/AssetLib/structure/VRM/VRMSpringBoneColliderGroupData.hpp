#pragma once
#include <vector>
#include <string>

#include "UnityEngine/Vector3.hpp"
#include "VRMSpringBoneColliderData.hpp"
#include "AssetLib/structure/node.hpp"

namespace AssetLib::Structure::VRM
{
    struct VRMSpringBoneColliderGroupData
    {
        AssetLib::Structure::Node* nodeRef;
        std::vector<VRMSpringBoneColliderData> colliders;
    };
};