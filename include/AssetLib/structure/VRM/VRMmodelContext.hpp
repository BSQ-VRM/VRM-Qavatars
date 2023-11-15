#pragma once

#include "../modelContext.hpp"
#include "vrm/VRMDescriptor.hpp"

namespace AssetLib::Structure::VRM
{
    //Same as ModelContext but contains extra fields for the VRM spec
    class VRMModelContext : virtual public ModelContext
    {
        public:
        VRMModelContext(ModelContext&& modelContext) : ModelContext(std::move(modelContext)) {

        }
        std::optional<VRMC_VRM_0_0::Vrm> vrm0;
        std::optional<VRMC_VRM_1_0::Vrm> vrm1;
    };
};