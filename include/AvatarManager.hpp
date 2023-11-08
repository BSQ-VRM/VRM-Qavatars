#pragma once

#include "main.hpp"
#include "AssetLib/structure/VRM/VRMmodelContext.hpp"

namespace VRMQavatars {
    class AvatarManager
    {
        public:
        static AssetLib::Structure::VRM::VRMModelContext* currentContext;
        static void SetContext(AssetLib::Structure::VRM::VRMModelContext* context);
    };
};