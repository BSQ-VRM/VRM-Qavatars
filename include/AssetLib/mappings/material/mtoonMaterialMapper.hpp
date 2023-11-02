#pragma once
#include "vrmIncludes.hpp"
#include "UnityEngine/Material.hpp"
#include "AssetLib/shaders/ShaderSO.hpp"

namespace AssetLib::Mappings::Materials {
    class MToonMaterialMapper
    {
    public:
        static UnityEngine::Material* map(VRMC_VRM_0_0::Material material, VRMData::ShaderSO* shaderData);
    };
}