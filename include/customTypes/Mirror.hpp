#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>
#include <UnityEngine/HumanBodyBones.hpp>
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/Transform.hpp>

namespace VRMQavatars
{
    class Mirror : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars", Mirror, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(Mirror);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(Mirror);
    public:

        DECLARE_INSTANCE_METHOD(void, Update);

    public:
        UnityEngine::Transform* camera;
        UnityEngine::Transform* target;
        int lastTrack;
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::Mirror);
