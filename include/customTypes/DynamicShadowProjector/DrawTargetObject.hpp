#pragma once

#include <UnityEngine/LayerMask.hpp>
#include <UnityEngine/Material.hpp>

#include "enums.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"

namespace DynamicShadowProjector
{
    class DrawTargetObject : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("DynamicShadowProjector", DrawTargetObject, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(DrawTargetObject);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(DrawTargetObject);
    public:

        DECLARE_INSTANCE_METHOD(void, Awake);
        DECLARE_INSTANCE_METHOD(void, OnDestroy);

        DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, m_target);
        DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, m_targetDirection);
        DECLARE_INSTANCE_FIELD(UnityEngine::LayerMask, m_layerMask);
        DECLARE_INSTANCE_FIELD(TextureAlignment, m_textureAlignment);
        DECLARE_INSTANCE_FIELD(UpdateFunction, m_updateFunction);
        DECLARE_INSTANCE_FIELD(UnityEngine::Material*, m_shadowShader);

    public:
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(DynamicShadowProjector::DrawTargetObject);