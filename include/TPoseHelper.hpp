#pragma once
#include "UnityEngine/Avatar.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/HumanPose.hpp"

namespace VRMQavatars
{
    struct TransData
    {
        SafePtrUnity<UnityEngine::Transform> trans;
        UnityEngine::Vector3 position;
        UnityEngine::Vector3 eulerRotation;
        UnityEngine::Vector3 localScale;
    };
    class TPoseHelper
    {
    public:
        static void SavePose(UnityEngine::Transform* root);
        static void LoadPose();
    private:
        static std::vector<TransData> originalPositions;
    };
}
