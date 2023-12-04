#pragma once
#include <sombrero/shared/FastVector3.hpp>

namespace AssetLib::Structure::VRM
{
    struct VRMSpringBoneColliderData
    {
        Sombrero::FastVector3 offset;
        float radius;
    };
};