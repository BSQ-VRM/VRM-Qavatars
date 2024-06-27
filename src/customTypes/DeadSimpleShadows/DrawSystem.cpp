#include "customTypes/DeadSimpleShadows/DrawSystem.hpp"

#include "System/Collections/Generic/List_1.hpp"
#include "UnityEngine/Rendering/CommandBuffer.hpp"
#include "UnityEngine/Rendering/RenderTargetIdentifier.hpp"
#include "UnityEngine/Rendering/CameraEvent.hpp"
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/zzzz__Vector3_def.hpp"
#include "beatsaber-hook/shared/utils/byref.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "customTypes/DeadSimpleShadows/DrawSystem.hpp"
#include "main.hpp"
#include "sombrero/shared/Vector3Utils.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/HideFlags.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/GL.hpp"
#include <vector>

namespace DeadSimpleShadows
{
    CP_SDK_IL2CPP_INHERIT_INIT(DrawSystem);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(DrawSystem)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(DrawSystem)
    {

    }

    void DrawSystem::Cleanup()
    {
        // We create this mesh so that OnWillRenderObject will always be called.
        auto meshFilter = GetComponent<UnityEngine::MeshFilter*>();
        if (!meshFilter->sharedMesh || meshFilter->sharedMesh->name != "DrawMesh" || meshFilter->sharedMesh->bounds.size.x < 999.0f)
        {
            auto mesh = UnityEngine::Mesh::New_ctor();
            mesh->name = "DrawMesh";
            mesh->hideFlags = UnityEngine::HideFlags::HideAndDontSave;
            mesh->bounds = UnityEngine::Bounds(UnityEngine::Vector3::get_zero(), UnityEngine::Vector3(9999999, 9999999, 9999999));
            meshFilter->sharedMesh = mesh;
        }

        if (!_camera)
            _camera = GetComponent<UnityEngine::Camera*>();

        if (_shadowBuffer == nullptr)
            return;

        // Ensure that we do not overfill the camera with buffers.
        _camera->RemoveCommandBuffer(UnityEngine::Rendering::CameraEvent::BeforeImageEffectsOpaque, _shadowBuffer);
    }

    void DrawSystem::OnDisable()
    {
        Cleanup();
    }

    void DrawSystem::OnEnable()
    {
        Cleanup();
    }
    
    void DrawSystem::OnWillRenderObject()
    {
        auto render = gameObject->activeInHierarchy && enabled;
        if (!render || !_camera)
        {
            Cleanup();
            return;
        }

        int textureWidth = (int)(textureSize * (width / height));
        int textureHeight = (int)textureSize;

        if (!_renderTexture || _renderTexture->width != textureWidth || _renderTexture->height != textureHeight || _camera->aspect != (width / height))
        {
            if (_renderTexture)
                _renderTexture->Release();
            _renderTexture = UnityEngine::RenderTexture::New_ctor(textureWidth, textureHeight, 24, UnityEngine::RenderTextureFormat::ARGB32);
            _renderTexture->name = "Shadow Texture";
            _renderTexture->Create();
            _camera->orthographicSize = height / 2.0f;
            _camera->aspect = width / height;
            _camera->rect = UnityEngine::Rect(0, 0, width, height);
            shadowMaterial->mainTexture = _renderTexture;
        }

        if (_camera->commandBufferCount > 0)
        {
            _camera->RemoveAllCommandBuffers();
        }

        auto cam = _camera;
        if (!cam)
            return;

        _shadowBuffer = UnityEngine::Rendering::CommandBuffer::New_ctor();
        _shadowBuffer->name = "Shadow Buffer";

        _shadowBuffer->SetRenderTarget(UnityEngine::Rendering::RenderTargetIdentifier::op_Implicit___UnityEngine__Rendering__RenderTargetIdentifier(_renderTexture));

        static auto clearRenderTarget = il2cpp_utils::resolve_icall<void, UnityEngine::Rendering::CommandBuffer*, int, ByRef<UnityEngine::Color>, float, uint>("UnityEngine.Rendering.CommandBuffer::ClearRenderTarget_Injected");
        auto color = UnityEngine::Color::get_clear();
        clearRenderTarget(_shadowBuffer, 7, byref(color), 1.0f, 0); // Ensure that the command buffer is transparent before rendering objects.

        // draw all objects to the command buffer
        for (auto drawObject : DrawRegistry::GetDrawObjects())
        {
            if (drawObject->responsibleMeshRenderers == nullptr) continue;
            for (int i = 0; i < drawObject->responsibleMeshRenderers->Count; i++) { 
                auto item = drawObject->responsibleMeshRenderers->get_Item(i);
                if(drawObject->excludedMeshRenderers->Contains(item)) continue;
                if (item)
                {
                    for (int i = 0; i < item->sharedMaterials.size(); i++)
                    {
                        _shadowBuffer->DrawRenderer(item, item->sharedMaterials[i], i, 0);
                    }
                }
            }
        }
        // add this command buffer to the pipeline
        cam->AddCommandBuffer(UnityEngine::Rendering::CameraEvent::BeforeImageEffectsOpaque, _shadowBuffer);

        cam->Render();

        shadowMaterial->mainTexture = _renderTexture;
    }
    
    UnityEngine::Bounds DrawSystem::GetIdealBoundingBox()
    {
        auto bounds = UnityEngine::Bounds();
        for (auto drawObject : DrawRegistry::GetDrawObjects())
        {
            if (drawObject->responsibleMeshRenderers == nullptr) continue;
            for (int i = 0; i < drawObject->responsibleMeshRenderers->Count; i++) { 
                auto item = drawObject->responsibleMeshRenderers->get_Item(i);
                if(drawObject->excludedMeshRenderers->Contains(item)) continue;
                auto rendBounds = item->get_bounds();
                if (bounds.size.Equals(UnityEngine::Vector3::get_zero()))
                {
                    bounds = rendBounds;
                }
                else
                {
                    bounds.Encapsulate(rendBounds.max);
                    bounds.Encapsulate(rendBounds.min);
                }
            }
        }
        return bounds;
    }
    
    void DrawSystem::DrawGLLine(UnityEngine::Vector3 one, UnityEngine::Vector3 two)
    {
        UnityEngine::GL::PushMatrix();
        lineMaterial->SetPass(0);
        UnityEngine::GL::Begin(1);
        UnityEngine::GL::Color(UnityEngine::Color::get_red());
        UnityEngine::GL::TexCoord2(0, 0);
        UnityEngine::GL::Vertex3(one.x, one.y, one.z);
        UnityEngine::GL::TexCoord2(1, 1);
        UnityEngine::GL::Vertex3(two.x, two.y, two.z);

        UnityEngine::GL::End();
        UnityEngine::GL::PopMatrix();
    }
    
    void DrawSystem::DrawFakeGizmos()
    {
        auto origins = GetRayOrigins();
        for (int i = 0; i < origins.size(); i++)
        {
            origins[i].y = std::max(origins[i].y, 0.05f);
        }
        DrawGLLine(origins[0], origins[1]);
        DrawGLLine(origins[1], origins[2]);
        DrawGLLine(origins[2], origins[3]);
        DrawGLLine(origins[3], origins[0]);

        auto raycasts = GetRaycastPositions();
        DrawGLLine(raycasts[0], raycasts[1]);
        DrawGLLine(raycasts[1], raycasts[2]);
        DrawGLLine(raycasts[2], raycasts[3]);
        DrawGLLine(raycasts[3], raycasts[0]);

        DrawGLLine(origins[0], raycasts[0]);
        DrawGLLine(origins[1], raycasts[1]);
        DrawGLLine(origins[2], raycasts[2]);
        DrawGLLine(origins[3], raycasts[3]);

        auto boundingBox = GetIdealBoundingBox();

        auto bottom = boundingBox.max;
        bottom.y += 0.02f;
        auto top = boundingBox.min;

        auto bottomL = UnityEngine::Vector3(bottom.x, bottom.y, bottom.z);
        auto bottomR = UnityEngine::Vector3(top.x, bottom.y, bottom.z);
        auto topL = UnityEngine::Vector3(bottom.x, bottom.y, top.z);
        auto topR = UnityEngine::Vector3(top.x, bottom.y, top.z);

        auto bottomLUP = UnityEngine::Vector3(bottom.x, top.y, bottom.z);
        auto bottomRUP = UnityEngine::Vector3(top.x, top.y, bottom.z);
        auto topLUP = UnityEngine::Vector3(bottom.x, top.y, top.z);
        auto topRUP = UnityEngine::Vector3(top.x, top.y, top.z);

        DrawGLLine(bottomL, bottomR);
        DrawGLLine(topL, topR);
        DrawGLLine(bottomL, topL);
        DrawGLLine(bottomR, topR);

        DrawGLLine(bottomLUP, bottomRUP);
        DrawGLLine(topLUP, topRUP);
        DrawGLLine(bottomLUP, topLUP);
        DrawGLLine(bottomRUP, topRUP);

        DrawGLLine(bottomL, bottomLUP);
        DrawGLLine(bottomR, bottomRUP);
        DrawGLLine(topL, topLUP);
        DrawGLLine(topR, topRUP);
    }
    
    void DrawSystem::OnRenderObject()
    {
        auto raycasts = GetRaycastPositions();
        UnityEngine::GL::PushMatrix();
        shadowMaterial->SetPass(0);
        UnityEngine::GL::Begin(7);
        UnityEngine::GL::Color(UnityEngine::Color::get_red());
        UnityEngine::GL::TexCoord2(0, 1);
        UnityEngine::GL::Vertex3(raycasts[0].x, raycasts[0].y, raycasts[0].z);
        UnityEngine::GL::TexCoord2(1, 1);
        UnityEngine::GL::Vertex3(raycasts[1].x, raycasts[1].y, raycasts[1].z);
        UnityEngine::GL::TexCoord2(1, 0);
        UnityEngine::GL::Vertex3(raycasts[2].x, raycasts[2].y, raycasts[2].z);
        UnityEngine::GL::TexCoord2(0, 0);
        UnityEngine::GL::Vertex3(raycasts[3].x, raycasts[3].y, raycasts[3].z);

        UnityEngine::GL::End();
        UnityEngine::GL::PopMatrix();

        if (!drawGizmos) return;
        DrawFakeGizmos();
    }
    
    ArrayW<UnityEngine::Vector3> DrawSystem::GetRayOrigins()
    {
        auto right = Sombrero::FastVector3(transform->right) * (width / 2.0f);
        auto up = Sombrero::FastVector3(transform->up) * (height / 2.0f);
        auto tl = -right + up + transform->position;
        auto tr = right + up + transform->position;
        auto bl = -right + -up + transform->position;
        auto br = right + -up + transform->position;
        auto array = ArrayW<UnityEngine::Vector3>(4);
        array[0] = tl;
        array[1] = tr;
        array[2] = br;
        array[3] = bl;
        return array;
    }
    
    ArrayW<UnityEngine::Vector3> DrawSystem::GetRaycastPositions()
    {
        auto origins = GetRayOrigins();
        for (int i = 0; i < origins.size(); i++)
        {
            origins[i].y = std::max(origins[i].y, 0.05f);
        }
        auto raycastPositions = ArrayW<UnityEngine::Vector3>(origins.size());
        for (int i = 0; i < origins.size(); i++)
        {
            auto origin = Sombrero::FastVector3(origins[i]);
            UnityEngine::RaycastHit hitInfo;
            auto hit = UnityEngine::Physics::Raycast(origin, transform->forward, byref(hitInfo), 1000.0f);
            if (hit)
            {
                raycastPositions[i] = Sombrero::FastVector3(hitInfo.point) + (Sombrero::FastVector3::up() * 0.01f);
            }
            else
            {
                raycastPositions[i] = origin + Sombrero::FastVector3(transform->forward) * 1000.0f;
            }
        }
        return raycastPositions;
    }


    std::vector<DrawObject*> DrawRegistry::drawObjs;

    void DrawRegistry::Add(DrawObject *o)
    {
        drawObjs.push_back(o);
    }

    void DrawRegistry::Remove(DrawObject *o)
    {
        drawObjs.erase(std::remove(drawObjs.begin(), drawObjs.end(), o), drawObjs.end());
    }

    std::vector<DrawObject*> DrawRegistry::GetDrawObjects()
    {
        return drawObjs;
    }
}