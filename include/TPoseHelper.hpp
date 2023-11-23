#pragma once
#include "UnityEngine/Avatar.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/HumanPose.hpp"

namespace VRMQavatars
{
    class TPoseHelper
    {
        UnityEngine::HumanPose LoadPose();
    public:
        void ApplyTpose(UnityEngine::Avatar* avatar, UnityEngine::Transform* root);
    };
}
