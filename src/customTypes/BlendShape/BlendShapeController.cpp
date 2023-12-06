#include "customTypes/BlendShape/BlendShapeController.hpp"

#include <GlobalNamespace/OVRInput_Button.hpp>
#include <UnityEngine/Time.hpp>
#include <UnityEngine/WaitForSeconds.hpp>

#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"

#include "AvatarManager.hpp"

#include "UnityEngine/Animator.hpp"
#include "UnityEngine/HumanBodyBones.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/Bounds.hpp"
#include "UnityEngine/Random.hpp"

#include "GlobalNamespace/OVRInput.hpp"

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

    custom_types::Helpers::Coroutine BlendShapeController::AutoBlinkCoroutine()
    {
        while(true)
        {
            constexpr float minTime = 1.5f;
            auto config = Config::ConfigManager::GetBlendshapeSettings();
            const float toWait = UnityEngine::Time::get_time() + minTime + (UnityEngine::Random::get_value() * config.waitTime);
            while(UnityEngine::Time::get_time() < toWait)
            {
                co_yield nullptr;
            }
            config = Config::ConfigManager::GetBlendshapeSettings();
            if(config.autoBlink && allowAutoBlink)
            {
                float blinkValue = 0.0f;

                constexpr float closeSpeed = 9.0f;
                constexpr float openSpeed = 9.0f;

                constexpr float closeDuration = 0.15f;
                //Animate closing
                while(true)
                {
                    blinkValue += UnityEngine::Time::get_deltaTime() * closeSpeed;
                    if (blinkValue >= 1.0f || !allowAutoBlink)
                    {
                        break;
                    }
                    blendShapeValues[blendShapePresetMappings[AssetLib::Structure::VRM::Blink].idx] = blinkValue;
                    co_yield nullptr;
                }
                //Ensure full blink
                blinkValue = 1.0f;
                blendShapeValues[blendShapePresetMappings[AssetLib::Structure::VRM::Blink].idx] = blinkValue;

                //Wait for duration of blink
                co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(closeDuration));

                //Animate Opening
                while(true)
                {
                    blinkValue -= UnityEngine::Time::get_deltaTime() * openSpeed;
                    if (blinkValue < 0.0f || !allowAutoBlink)
                    {
                        break;
                    }
                    blendShapeValues[blendShapePresetMappings[AssetLib::Structure::VRM::Blink].idx] = blinkValue;
                    co_yield nullptr;
                }
                //Ensure no more blinking
                blinkValue = 0.0f;
                blendShapeValues[blendShapePresetMappings[AssetLib::Structure::VRM::Blink].idx] = blinkValue;
            }
        }
        co_return;
    }

    void BlendShapeController::Init()
    {
        init = false;
        const auto animator = GetComponent<UnityEngine::Animator*>();

        leftEye = animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftEye);
        rightEye = animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightEye);

        const auto nodes = AvatarManager::currentContext->nodes;
        for (int i = 0; i < nodes.size(); ++i)
        {
            const auto node = nodes[i];
            if(node->mesh.has_value())
            {
                renderers.push_back(node->gameObject->GetComponent<UnityEngine::SkinnedMeshRenderer*>());
            }
        }

        const auto blendShapeMaster = AvatarManager::currentContext->blendShapeMaster;
        for (auto blendShape : blendShapeMaster->groups)
        {
            blendShapePresetMappings[blendShape.preset] = blendShape;
            blendShapeMappings[blendShape.idx] = blendShape;
            blendShapeValues[blendShape.idx] = 0.0f;
            blendShapeTargetValues[blendShape.idx] = 0.0f;
            reverseMappings[blendShape.name] = blendShape.idx;
        }
        init = true;

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(AutoBlinkCoroutine()));
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

    inline bool IsButtonDown(const int& buttonIdx, const int controller) {
        if(buttonIdx <= 0) return false;
        const auto button = buttons[buttonIdx];
        if(controller == 2)
            return GlobalNamespace::OVRInput::Get(button, controllers[0]) || GlobalNamespace::OVRInput::Get(button, controllers[1]);
        return GlobalNamespace::OVRInput::Get(button, controllers[controller]);
    }

    bool SkipBlendShape(const AssetLib::Structure::VRM::BlendShapePreset preset, const BlendshapeSettings& settings, bool allow)
    {
        if(settings.autoBlink && allow)
        {
            return preset == AssetLib::Structure::VRM::Blink;
        }
        return false;
    }

    void BlendShapeController::Update()
    {
        if(!init) return;
        bool any = false; //Used to determine if we should lerp to neutral preset
        auto triggerConfig = Config::ConfigManager::GetControllerTriggerSettings();
        auto blendShapeConfig = Config::ConfigManager::GetBlendshapeSettings();

        //Reset All to 0
        for (auto const& [key, val] : blendShapeTargetValues)
        {
            blendShapeTargetValues[key] = 0.0f;
        }

        //Check for inputs
        if(triggerConfig.A != "None")
        {
            const bool val = IsButtonDown(3, 1);
            any |= val; //If any is false and the output is true then set it

            const auto preset = reverseMappings[triggerConfig.A];
            blendShapeTargetValues[preset] = val ? 1.0f : 0.0f;
        }

        if(triggerConfig.B != "None")
        {
            const bool val = IsButtonDown(4, 1);
            any |= val; //If any is false and the output is true then set it

            const auto preset = reverseMappings[triggerConfig.B];
            blendShapeTargetValues[preset] = val ? 1.0f : 0.0f;
        }

        if(triggerConfig.X != "None")
        {
            const bool val = IsButtonDown(3, 0);
            any |= val; //If any is false and the output is true then set it

            const auto preset = reverseMappings[triggerConfig.X];
            blendShapeTargetValues[preset] = val ? 1.0f : 0.0f;
        }

        if(triggerConfig.Y != "None")
        {
            const bool val = IsButtonDown(4, 0);
            any |= val; //If any is false and the output is true then set it

            const auto preset = reverseMappings[triggerConfig.Y];
            blendShapeTargetValues[preset] = val ? 1.0f : 0.0f;
        }

        if(triggerConfig.LGrip != "None")
        {
            const bool val = IsButtonDown(1, 0);
            any |= val; //If any is false and the output is true then set it

            const auto preset = reverseMappings[triggerConfig.LGrip];
            blendShapeTargetValues[preset] = val ? 1.0f : 0.0f;
        }

        if(triggerConfig.RGrip != "None")
        {
            const bool val = IsButtonDown(1, 1);
            any |= val; //If any is false and the output is true then set it

            const auto preset = reverseMappings[triggerConfig.RGrip];
            blendShapeTargetValues[preset] = val ? 1.0f : 0.0f;
        }

        if(triggerConfig.LTrigger != "None")
        {
            const bool val = IsButtonDown(2, 0);
            any |= val; //If any is false and the output is true then set it

            const auto preset = reverseMappings[triggerConfig.LTrigger];
            blendShapeTargetValues[preset] = val ? 1.0f : 0.0f;
        }

        if(triggerConfig.RTrigger != "None")
        {
            const bool val = IsButtonDown(2, 1);
            any |= val; //If any is false and the output is true then set it

            const auto preset = reverseMappings[triggerConfig.RTrigger];
            blendShapeTargetValues[preset] = val ? 1.0f : 0.0f;
        }

        const auto neutralPreset = reverseMappings[blendShapeConfig.neutralExpression];

        blendShapeTargetValues[neutralPreset] = any ? 0.0f : 1.0f;

        allowAutoBlink = !any;

        //Lerp Values

        for (auto const& [key, val] : blendShapeTargetValues)
        {
            if(SkipBlendShape(blendShapeMappings[key].preset, blendShapeConfig, allowAutoBlink) && !any) continue;
            blendShapeValues[key] = UnityEngine::Mathf::Lerp(blendShapeValues[key], val, UnityEngine::Time::get_deltaTime() * 10.0f);
        }

        //Set Values

        for (auto const& [key, val] : blendShapeValues)
        {
            SetBlendshape(key, val * 100.0f);
        }

        if(blendShapeConfig.mockEyeMovement)
        {
            if(UnityEngine::Time::get_time() > time)
            {
                //Switch eye positions
                Sombrero::FastVector3 unitPos = UnityEngine::Random::get_insideUnitSphere(); //random lookdir
                eyeVec.x = unitPos.x * 2.5f; //vert
                eyeVec.y = unitPos.y * 5.0f; //horz
                eyeVec.z = 0.0f;

                time = UnityEngine::Time::get_time() + UnityEngine::Random::Range(0.75f, 2.5f);
            }
        }
        else
        {
            eyeVec = Sombrero::FastVector3::zero();
        }

        if(leftEye != nullptr && rightEye != nullptr)
        {
            auto leftAngle = UnityEngine::Quaternion::Lerp(leftEye->get_localRotation(), UnityEngine::Quaternion::Euler(eyeVec), UnityEngine::Time::get_deltaTime() * 15.0f);
            auto rightAngle = UnityEngine::Quaternion::Lerp(rightEye->get_localRotation(), UnityEngine::Quaternion::Euler(eyeVec), UnityEngine::Time::get_deltaTime() * 15.0f);
            leftEye->set_localRotation(leftAngle);
            rightEye->set_localRotation(rightAngle);
        }
    }

    void BlendShapeController::OnDestroy()
    {

    }

    float smoothstep(float edge0, float edge1, float x, float max) {
        // Scale, bias and saturate x to 0..1 range
        x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, max);
        // Evaluate polynomial
        return x * x * (3 - 2 * x);
    }

    void BlendShapeController::SetBlendshape(const int preset, const float value)
    {
        auto [binds, isBinary, materialBinds, name, idx, presetName] = blendShapeMappings[preset];
        for (int i = 0; i < binds.size(); i++)
        {
            const auto bind = binds[i];
            for (int k = 0; k < renderers.size(); ++k)
            {
                if(k == bind.mesh && renderers[k]->get_sharedMesh()->get_blendShapeCount() > bind.index)
                {
                    const auto weight = (bind.weight / 100.0f);
                    const float newValue = isBinary ? smoothstep(5.0f, 10.0f, value, weight) : value * weight;
                    renderers[k]->SetBlendShapeWeight(bind.index, newValue);
                }
            }
        }
    }
};