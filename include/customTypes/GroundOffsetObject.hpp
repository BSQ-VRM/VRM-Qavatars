#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>
#include <UnityEngine/MonoBehaviour.hpp>

namespace VRMQavatars
{
    class GroundOffsetObject : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars", GroundOffsetObject, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(GroundOffsetObject);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(GroundOffsetObject);
    public:

        DECLARE_INSTANCE_METHOD(void, Update);
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::GroundOffsetObject);
