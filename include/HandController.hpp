#pragma once

#include "UnityEngine/Animator.hpp"

namespace VRMQavatars
{
    class HandController
    {
        public:
        static void ApplyHandPose(UnityEngine::Animator* animator, std::string pose);
        static std::vector<std::string> SplitPose(std::string pose);
        static std::vector<float> ParseRotations(std::vector<std::string> pose);
    };
};