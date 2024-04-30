#pragma once

#include "vrm/vrmIncludes.hpp"
#include <concepts>
#include <type_traits>

namespace AssetLib
{
    template<typename T>
    concept VRMVersion = std::same_as<VRMC_VRM_0_0::Vrm, T> || std::same_as<VRMC_VRM_1_0::Vrm, T>;
};