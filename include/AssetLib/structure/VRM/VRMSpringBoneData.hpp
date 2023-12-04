#pragma once
#include <vector>
#include <string>
#include <sombrero/shared/FastVector3.hpp>

namespace AssetLib::Structure::VRM
{
    struct VRMSpringBoneData
    {
        std::string comment;
        float stiffiness{};
        float gravityPower{};
        Sombrero::FastVector3 gravityDir{};
        float dragForce{};
        uint32_t center{};
        float hitRadius{};
        std::vector<uint32_t> bones{};
        std::vector<uint32_t> colliderGroups{};
    };
};