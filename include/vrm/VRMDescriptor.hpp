#pragma once

#include "vrm/vrmIncludes.hpp"

namespace VRMQavatars
{ 
    struct VRMDescriptor
    {
        std::string filePath;
        std::string name;
        std::string author;

        std::optional<VRMC_VRM_0_0::Vrm> vrm0;
        std::optional<VRMC_VRM_1_0::Vrm> vrm1;
    };

    struct AvatarComparator
    {
        constexpr bool operator()(const VRMDescriptor& lhs, const VRMDescriptor& rhs) const
        {
            return lhs.name < rhs.name;
        }
    };
};