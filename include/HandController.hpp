#pragma once

#include "UnityEngine/Animator.hpp"

namespace VRMQavatars
{
    class HandController
    {
        public:
        static void ApplyHandPose(UnityEngine::Animator* animator, std::string pose);
        static std::vector<std::string> SplitPose(const std::string& pose);
        static std::vector<float> ParseRotations(const std::vector<std::string>& pose);
        static int GetValue(const std::string& pose, const int idx);
        static std::string SetValue(const std::string& pose, const int idx, const int value);
    };
};