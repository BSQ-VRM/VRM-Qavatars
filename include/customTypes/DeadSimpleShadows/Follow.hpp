#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>
#include <UnityEngine/MonoBehaviour.hpp>

#include "DrawSystem.hpp"

namespace DeadSimpleShadows
{
    class Follow : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("DeadSimpleShadows", Follow, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(Follow);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(Follow);
    public:

        DECLARE_INSTANCE_METHOD(void, Update);
        DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, target);
        DECLARE_INSTANCE_FIELD(DrawSystem*, drawSystem);
        DECLARE_INSTANCE_FIELD(bool, useMeshBoundingBox);
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(DeadSimpleShadows::Follow);
