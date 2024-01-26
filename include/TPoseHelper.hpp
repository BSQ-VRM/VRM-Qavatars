#pragma once
#include <sombrero/shared/FastVector3.hpp>

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
