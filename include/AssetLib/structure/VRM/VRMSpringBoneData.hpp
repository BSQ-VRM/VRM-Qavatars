#pragma once
#include <vector>
#include <string>

#include "UnityEngine/Vector3.hpp"

namespace AssetLib::Structure::VRM
{
    struct VRMSpringBoneData
    {
        std::string comment;
        float stiffiness{};
        float gravityPower{};
        UnityEngine::Vector3 gravityDir{};
        float dragForce{};
        uint32_t center{};
        float hitRadius{};
        std::vector<uint32_t> bones{};
        std::vector<uint32_t> colliderGroups{};
    };
};