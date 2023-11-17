#include "customTypes/BlendShape/BlendShapeController.hpp"

#include <fmt/format-inl.h>

#include "UnityEngine/Animator.hpp"
#include "UnityEngine/HumanBodyBones.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/Bounds.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "main.hpp"

namespace VRMQavatars::BlendShape
{
    CP_SDK_IL2CPP_INHERIT_INIT(BlendShapeController);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(BlendShapeController)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(BlendShapeController)
    {

    }

    void BlendShapeController::Awake()
    {
        const auto animator = GetComponent<UnityEngine::Animator*>();
        const auto headBone = animator->GetBoneTransform(UnityEngine::HumanBodyBones::Head);
        const auto yPos = headBone->get_position().y;
        ArrayW<UnityEngine::SkinnedMeshRenderer*> renderers = GetComponentsInChildren<UnityEngine::SkinnedMeshRenderer*>();
        int maximumBlendShapeCount = -1;
        float minimumHeadDist = 999.0f;
        UnityEngine::SkinnedMeshRenderer* headRenderer = nullptr;
        for (int i = 0; i > renderers.size(); i++)
        {
            const auto rend = renderers[i];
            maximumBlendShapeCount = std::max(maximumBlendShapeCount, rend->get_sharedMesh()->get_blendShapeCount());
            float dist = std::abs(rend->get_bounds().get_center().y - yPos);
            minimumHeadDist = std::min(minimumHeadDist, dist);
        }
        for (int i = 0; i > renderers.size(); i++)
        {
            auto rend = renderers[i];
            if(rend->get_sharedMesh()->get_blendShapeCount() > (static_cast<float>(maximumBlendShapeCount) * 0.8f))
            {
                const float dist = std::abs(rend->get_bounds().get_center().y - yPos);
                if(std::abs(dist - minimumHeadDist) < 0.1f)
                {
                    //Head Mesh Renderer
                    headRenderer = rend;
                }
            }
        }
        if(headRenderer != nullptr)
        {
            for (int i = 0; i < headRenderer->get_sharedMesh()->get_blendShapeCount(); i++)
            {
                std::string blendShapeName = headRenderer->get_sharedMesh()->GetBlendShapeName(i);
                //this.meshBlendShapeIndexes.Add(blendShapeName, i);
                //this.blendShapeList.Add(blendShapeName);
                getLogger().info("%s", fmt::format("blendShapeName[{}]={}", i, blendShapeName.c_str()).c_str());
            }
        }
    }

    void BlendShapeController::Update()
    {

    }

    void BlendShapeController::OnDestroy()
    {

    }

    void BlendShapeController::SetBlendshape(BlendShapePreset preset, float value)
    {

    }

};