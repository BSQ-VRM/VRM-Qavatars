#pragma once

#include <UnityEngine/Camera.hpp>
#include <UnityEngine/Color.hpp>
#include <UnityEngine/LayerMask.hpp>
#include <UnityEngine/Material.hpp>
#include <UnityEngine/RenderTexture.hpp>
#include <UnityEngine/Rendering/CommandBuffer.hpp>

#include "enums.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"

namespace DynamicShadowProjector
{
    class ShadowTextureRenderer : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("DynamicShadowProjector", ShadowTextureRenderer, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(ShadowTextureRenderer);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(ShadowTextureRenderer);
    public:

        DECLARE_INSTANCE_METHOD(void, Awake);
        DECLARE_INSTANCE_METHOD(void, OnDestroy);

        DECLARE_INSTANCE_FIELD(UnityEngine::Color, m_shadowColor);
        DECLARE_INSTANCE_METHOD(UnityEngine::Color, get_shadowColor);
        DECLARE_INSTANCE_METHOD(void, set_shadowColor, UnityEngine::Color);

        DECLARE_INSTANCE_METHOD(UnityEngine::LayerMask, get_cameraCullingMask);
        DECLARE_INSTANCE_METHOD(void, set_cameraCullingMask, UnityEngine::LayerMask);

        DECLARE_INSTANCE_FIELD(UnityEngine::RenderTexture*, shadowTexture);

    public:
        void Initialize();

        TextureMultiSample m_multiSampling = x8;
        TextureSuperSample m_superSampling = sx1;
        int m_textureWidth = 2048;
        int m_textureHeight = 2048;
        int m_blurLevel = 1;
        float m_blurSize = 3;
        UnityEngine::Material* m_blurShader;
        UnityEngine::Material* m_downsampleShader;
        UnityEngine::Material* m_eraseShadowShader;

        UnityEngine::Projector* m_projector;
        UnityEngine::Material m_projectorMaterial;
        UnityEngine::Camera* m_camera;
        UnityEngine::Rendering::CommandBuffer* m_postProcessCommands = nullptr;
        bool m_isTexturePropertyChanged;
        bool m_isVisible = false;
        bool m_shadowTextureValid = false;



        //Static
        static int s_blurOffsetHParamID;
        static int s_blurOffsetVParamID;
        static int s_blurWeightHParamID;
        static int s_blurWeightVParamID;
        static int s_shadowColorParamID;
        static int s_blitSrcTexParamID;
        static int s_blitSrcTexelSizeParamID;
        static int s_shadowTexParamID;
        static int s_mipLevelParamID;
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(DynamicShadowProjector::ShadowTextureRenderer);