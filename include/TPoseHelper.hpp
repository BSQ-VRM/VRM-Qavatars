#pragma once
#include <sombrero/shared/FastVector3.hpp>

// Ensure HumanBone/SkeletonBone are fully defined before Avatar.hpp triggers HumanDescription_impl
#include "UnityEngine/zzzz__HumanBone_impl.hpp"
#include "UnityEngine/zzzz__SkeletonBone_impl.hpp"
#include "UnityEngine/Avatar.hpp"

#include "UnityEngine/Transform.hpp"

namespace VRMQavatars
{
    struct TransData
    {
        SafePtrUnity<UnityEngine::Transform> trans;
        Sombrero::FastVector3 position;
        Sombrero::FastVector3 eulerRotation;
        Sombrero::FastVector3 localScale;
    };
    class TPoseHelper
    {
    public:
        static void SavePose(UnityEngine::Transform* root);
        static void LoadPose();
        static void LoadPose(const UnityEngine::Transform* trans);
    private:
        static std::vector<TransData> originalPositions;
    };
}
