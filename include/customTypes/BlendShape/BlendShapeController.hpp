#pragma once

#include "BlendShapePreset.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

namespace VRMQavatars::BlendShape {
    class BlendShapeController : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.BlendShape", BlendShapeController, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(BlendShapeController);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(BlendShapeController);
    public:

        DECLARE_INSTANCE_METHOD(void, Awake);
        DECLARE_INSTANCE_METHOD(void, OnDestroy);
        DECLARE_INSTANCE_METHOD(void, Update);
        DECLARE_INSTANCE_METHOD(void, SetBlendshape, BlendShapePreset preset, float value);
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::BlendShape::BlendShapeController);