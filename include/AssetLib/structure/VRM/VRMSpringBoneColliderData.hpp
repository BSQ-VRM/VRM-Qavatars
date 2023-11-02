#pragma once
#include <vector>
#include <string>

#include "UnityEngine/Vector3.hpp"

namespace AssetLib::Structure::VRM
{
    struct VRMSpringBoneColliderData
    {
        UnityEngine::Vector3 offset;
        float radius;
    };
};