#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>
#include <UnityEngine/MonoBehaviour.hpp>

#include "DrawObject.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Rendering/CommandBuffer.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Bounds.hpp"
#include "UnityEngine/Vector3.hpp"

namespace DeadSimpleShadows
{
    class DrawSystem : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("DeadSimpleShadows", DrawSystem, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(DrawSystem);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(DrawSystem);
    public:

        DECLARE_INSTANCE_FIELD(float, width);
        DECLARE_INSTANCE_FIELD(float, height);
        DECLARE_INSTANCE_FIELD(float, textureSize);
        DECLARE_INSTANCE_FIELD(UnityEngine::Material*, shadowMaterial);
        DECLARE_INSTANCE_FIELD(UnityEngine::Material*, lineMaterial);
        DECLARE_INSTANCE_FIELD(bool, drawGizmos);

        DECLARE_INSTANCE_FIELD(UnityEngine::Camera*, _camera);
        DECLARE_INSTANCE_FIELD(UnityEngine::Rendering::CommandBuffer*, _shadowBuffer);
        DECLARE_INSTANCE_FIELD(UnityEngine::RenderTexture*, _renderTexture);

        DECLARE_INSTANCE_METHOD(void, Cleanup);
        DECLARE_INSTANCE_METHOD(void, OnDisable);
        DECLARE_INSTANCE_METHOD(void, OnEnable);
        DECLARE_INSTANCE_METHOD(void, OnWillRenderObject);
        DECLARE_INSTANCE_METHOD(UnityEngine::Bounds, GetIdealBoundingBox);
        DECLARE_INSTANCE_METHOD(void, DrawGLLine, UnityEngine::Vector3 one, UnityEngine::Vector3 two);
        DECLARE_INSTANCE_METHOD(void, DrawFakeGizmos);
        DECLARE_INSTANCE_METHOD(void, OnRenderObject);
        DECLARE_INSTANCE_METHOD(ArrayW<UnityEngine::Vector3>, GetRayOrigins);
        DECLARE_INSTANCE_METHOD(ArrayW<UnityEngine::Vector3>, GetRaycastPositions);
    };

    class DrawRegistry
    {
        public:
            static void Add(DrawObject* o);
            static void Remove(DrawObject* o);
            static std::vector<DrawObject*> GetDrawObjects();
        private:
            static std::vector<DrawObject*> drawObjs;
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(DeadSimpleShadows::DrawSystem);
