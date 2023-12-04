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
        static void UpdateMirror();
        static void CreateMainMirror();
        static UnityEngine::GameObject* CreateMirror(Sombrero::FastVector3 position, Sombrero::FastVector3 rotation, Sombrero::FastVector2 size, int layer, float fov);
    private:
        static SafePtrUnity<UnityEngine::GameObject> mainMirror;
        static SafePtrUnity<UnityEngine::Camera> mirrorCamera;
        static SafePtrUnity<UnityEngine::MeshRenderer> mirrorRenderer;

    public:
        static SafePtrUnity<UnityEngine::Shader> mirrorShader;
    };
}