#pragma once

#include "UnityEngine/Animator.hpp"

namespace VRMQavatars
{
    class HandController
    {
        public:
        static void ApplyHandPose(UnityEngine::Animator* avatar, std::string pose);  
    };
};