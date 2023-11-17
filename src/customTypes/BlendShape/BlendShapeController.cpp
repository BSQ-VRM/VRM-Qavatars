#include "customTypes/BlendShape/BlendShapeController.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"

#include "AvatarManager.hpp"
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

    void BlendShapeController::Init()
    {
        getLogger().info("blendShape x1");
        init = false;
        const auto animator = GetComponent<UnityEngine::Animator*>();
        const auto headBone = animator->GetBoneTransform(UnityEngine::HumanBodyBones::Head);
        const auto yPos = headBone->get_position().y;
        getLogger().info("blendShape x2");
        ArrayW<UnityEngine::SkinnedMeshRenderer*> renderers = GetComponentsInChildren<UnityEngine::SkinnedMeshRenderer*>();
        int maximumBlendShapeCount = -1;
        float minimumHeadDist = 999.0f;
        for (int i = 0; i < renderers.size(); i++)
        {
            getLogger().info("blendShape x3");
            const auto rend = renderers[i];
            maximumBlendShapeCount = std::max(maximumBlendShapeCount, rend->get_sharedMesh()->get_blendShapeCount());
            float dist = std::abs(rend->get_bounds().get_center().y - yPos);
            minimumHeadDist = std::min(minimumHeadDist, dist);
        }
        getLogger().info("blendShape x4");
        for (int i = 0; i < renderers.size(); i++)
        {
            getLogger().info("blendShape x5");
            auto rend = renderers[i];
            if(rend->get_sharedMesh()->get_blendShapeCount() > (static_cast<float>(maximumBlendShapeCount) * 0.8f))
            {
                getLogger().info("blendShape x6");
                const float dist = std::abs(rend->get_bounds().get_center().y - yPos);
                if(std::abs(dist - minimumHeadDist) < 0.1f)
                {
                    getLogger().info("blendShape x7 %s", static_cast<std::string>(rend->get_gameObject()->get_name()).c_str());
                    //Head Mesh Renderer
                    headRenderer = rend;
                }
            }
        }
        if(headRenderer != nullptr)
        {
            getLogger().info("blendShape x8");
            for (int i = 0; i < headRenderer->get_sharedMesh()->get_blendShapeCount(); i++)
            {
                getLogger().info("blendShape x9");
                std::string blendShapeName = headRenderer->get_sharedMesh()->GetBlendShapeName(i);
                //this.meshBlendShapeIndexes.Add(blendShapeName, i);
                //this.blendShapeList.Add(blendShapeName);
                getLogger().info("blendShape blendShapeName[%d]=%s", i, blendShapeName.c_str());
            }
        }
        getLogger().info("blendShape x10");

        auto blendShapeMaster = AvatarManager::currentContext->blendShapeMaster;
        getLogger().info("blendShape x11");
        for (auto blendShape : blendShapeMaster->groups)
        {
            getLogger().info("blendShape x12");
            blendShapeMappings[blendShape.preset] = blendShape;
        }
        init = true;
    }

    void BlendShapeController::Update()
    {
    }

    void BlendShapeController::OnDestroy()
    {

    }

    void BlendShapeController::SetBlendshape(const AssetLib::Structure::VRM::BlendShapePreset preset, const float value)
    {
        getLogger().info("set value %f", value);
        getLogger().info("set preset %d", static_cast<int>(preset));
        auto [binds, isBinary, materialBinds, name, presetName] = blendShapeMappings[preset];
        getLogger().info("set presetName %d", static_cast<int>(presetName));
        getLogger().info("set name %s", name.c_str());
        for (int i = 0; i < binds.size(); i++)
        {
            const auto bind = binds[i];
            headRenderer->SetBlendShapeWeight(bind.index, value);
        }
    }
};