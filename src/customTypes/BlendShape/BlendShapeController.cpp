#include "customTypes/BlendShape/BlendShapeController.hpp"

#include <GlobalNamespace/OVRInput_Button.hpp>
#include <UnityEngine/Time.hpp>

#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"

#include "AvatarManager.hpp"

#include "UnityEngine/Animator.hpp"
#include "UnityEngine/HumanBodyBones.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/Bounds.hpp"

#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"
#include "GlobalNamespace/OVRInput_RawButton.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "main.hpp"
#include "config/ConfigManager.hpp"

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
        init = false;
        const auto animator = GetComponent<UnityEngine::Animator*>();
        const auto headBone = animator->GetBoneTransform(UnityEngine::HumanBodyBones::Head);
        const auto yPos = headBone->get_position().y;
        ArrayW<UnityEngine::SkinnedMeshRenderer*> renderers = GetComponentsInChildren<UnityEngine::SkinnedMeshRenderer*>();
        int maximumBlendShapeCount = -1;
        float minimumHeadDist = 999.0f;
        for (int i = 0; i < renderers.size(); i++)
        {
            const auto rend = renderers[i];
            maximumBlendShapeCount = std::max(maximumBlendShapeCount, rend->get_sharedMesh()->get_blendShapeCount());
            float dist = std::abs(rend->get_bounds().get_center().y - yPos);
            minimumHeadDist = std::min(minimumHeadDist, dist);
        }
        for (int i = 0; i < renderers.size(); i++)
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

        auto blendShapeMaster = AvatarManager::currentContext->blendShapeMaster;
        for (auto blendShape : blendShapeMaster->groups)
        {
            blendShapeMappings[blendShape.preset] = blendShape;
            blendShapeValues[blendShape.preset] = 0.0f;
            blendShapeTargetValues[blendShape.preset] = 0.0f;
        }
        init = true;
    }

    const std::vector<GlobalNamespace::OVRInput::Button> buttons = {
        GlobalNamespace::OVRInput::Button::None,
        GlobalNamespace::OVRInput::Button::PrimaryHandTrigger,
        GlobalNamespace::OVRInput::Button::PrimaryIndexTrigger,
        GlobalNamespace::OVRInput::Button::One,
        GlobalNamespace::OVRInput::Button::Two,
        GlobalNamespace::OVRInput::Button::PrimaryThumbstickUp,
        GlobalNamespace::OVRInput::Button::PrimaryThumbstickDown,
        GlobalNamespace::OVRInput::Button::PrimaryThumbstickLeft,
        GlobalNamespace::OVRInput::Button::PrimaryThumbstickRight
    };
    const std::vector<GlobalNamespace::OVRInput::Controller> controllers = {
        GlobalNamespace::OVRInput::Controller::LTouch,
        GlobalNamespace::OVRInput::Controller::RTouch
    };

    inline bool IsButtonDown(const int& buttonIdx, int controller) {
        if(buttonIdx <= 0) return false;
        const auto button = buttons[buttonIdx];
        if(controller == 2)
            return GlobalNamespace::OVRInput::Get(button, controllers[0]) || GlobalNamespace::OVRInput::Get(button, controllers[1]);
        return GlobalNamespace::OVRInput::Get(button, controllers[controller]);
    }

    void BlendShapeController::Update()
    {
        bool any = false; //Used to determine if we should lerp to neutral preset
        auto triggerConfig = Config::ConfigManager::GetControllerTriggerSettings();
        if(true)
        {
            bool val = IsButtonDown(3, 1);
            any |= val; //If any is false and the output is true then set it

            auto preset = AssetLib::Structure::VRM::BlendShapePreset::Unknown;
            blendShapeTargetValues[preset] = val ? 1.0f : 0.0f;
        }

        auto neutralPreset = AssetLib::Structure::VRM::BlendShapePreset::Fun;

        blendShapeTargetValues[neutralPreset] = any ? 0.0f : 1.0f;


        //Lerp Values

        for (auto const& [key, val] : blendShapeTargetValues)
        {
            blendShapeValues[key] = UnityEngine::Mathf::Lerp(blendShapeValues[key], val, UnityEngine::Time::get_deltaTime() * 10.0f);
        }

        //Set Values

        for (auto const& [key, val] : blendShapeValues)
        {
            SetBlendshape(key, val * 100.0f);
        }
    }

    void BlendShapeController::OnDestroy()
    {

    }

    void BlendShapeController::SetBlendshape(const AssetLib::Structure::VRM::BlendShapePreset preset, const float value)
    {
        auto [binds, isBinary, materialBinds, name, presetName] = blendShapeMappings[preset];
        for (int i = 0; i < binds.size(); i++)
        {
            const auto bind = binds[i];
            headRenderer->SetBlendShapeWeight(bind.index, value);
        }
    }
};