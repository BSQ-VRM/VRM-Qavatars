#pragma once
#include <vector>
#include "VRMSpringBoneColliderData.hpp"
#include "AssetLib/structure/node.hpp"

namespace AssetLib::Structure::VRM
{
    struct VRMSpringBoneColliderGroupData
    {
        Node* nodeRef;
        std::vector<VRMSpringBoneColliderData> colliders;
    };
};