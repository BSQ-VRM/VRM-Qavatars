#pragma once
#include <UnityEngine/Camera.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/MeshRenderer.hpp>

class MirrorManager
{
public:
    static void UpdateMirror();
    static void CreateMainMirror();
    static UnityEngine::GameObject* CreateMirror(UnityEngine::Vector3 position, UnityEngine::Vector3 rotation, UnityEngine::Vector2 size, int layer);
private:
    static SafePtrUnity<UnityEngine::GameObject> mainMirror;
    static SafePtrUnity<UnityEngine::Camera> mirrorCamera;
    static SafePtrUnity<UnityEngine::MeshRenderer> mirrorRenderer;
};
