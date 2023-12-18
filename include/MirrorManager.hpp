#pragma once
#include <sombrero/shared/FastVector2.hpp>
#include <sombrero/shared/FastVector3.hpp>
#include <UnityEngine/Camera.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/MeshRenderer.hpp>

namespace VRMQavatars
{
    class MirrorManager
    {
    public:
        static void UpdateMirror(bool changedSize);
        static void CreateMainMirror();
        static UnityEngine::GameObject* CreateMirror(Sombrero::FastVector3 position, Sombrero::FastVector3 rotation, Sombrero::FastVector2 size, float aspect, int layer, float fov, UnityEngine::Color border, bool transparentBackground, bool addComponent, bool handle);
    public:
        static SafePtrUnity<UnityEngine::GameObject> mainMirror;
        static SafePtrUnity<UnityEngine::Camera> mirrorCamera;
        static SafePtrUnity<UnityEngine::MeshRenderer> mirrorRenderer;
        static SafePtrUnity<UnityEngine::Shader> mirrorShader;
    };
}