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
        getLogger().info("x");
        while(true)
        {
            getLogger().info("x1");
            const float minTime = 1.0f;
            auto config = Config::ConfigManager::GetBlendShapeSettings();
            float toWait = UnityEngine::Time::get_time() + minTime + (UnityEngine::Random::get_value() * config.waitTime);
            while(UnityEngine::Time::get_time() < toWait)
            {
                co_yield nullptr;
            }
            if(config.autoBlink)
            {
                float blinkValue = 0.0f;

                float closeSpeed = 10.0f;
                float openSpeed = 25.0f;

                float closeDuration = 0.05f;
                //Animate closing
                while(true)
                {
                    blinkValue += UnityEngine::Time::get_deltaTime() * closeSpeed;
                    if (blinkValue >= 1.0f)
                    {
                        break;
                    }
                    blendShapeTargetValues[AssetLib::Structure::VRM::Blink] = blinkValue;
                    co_yield nullptr;
                }
                //Ensure full blink
                blinkValue = 1.0f;
                blendShapeTargetValues[AssetLib::Structure::VRM::Blink] = blinkValue;

                //Wait for duration of blink
                co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(closeDuration));

                //Animate Opening
                while(true)
                {
                    blinkValue -= UnityEngine::Time::get_deltaTime() * openSpeed;
                    if (blinkValue < 0.0f)
                    {
                        break;
                    }
                    blendShapeTargetValues[AssetLib::Structure::VRM::Blink] = blinkValue;
                    co_yield nullptr;
                }
                //Ensure no more blinking
                blinkValue = 0.0f;
                blendShapeTargetValues[AssetLib::Structure::VRM::Blink] = blinkValue;
            }
        }
        co_return;
    }

    void BlendShapeController::Init()
    {
        init = false;
        const auto animator = GetComponent<UnityEngine::Animator*>();
        const auto headBone = animator->GetBoneTransform(UnityEngine::HumanBodyBones::Head);
        const auto yPos = headBone->get_position().y;

        leftEye = animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftEye);
        rightEye = animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightEye);

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
            reverseMappings[blendShape.name] = blendShape.preset;
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

    inline bool IsButtonDown(const int& buttonIdx, int controller) {
        if(buttonIdx <= 0) return false;
        const auto button = buttons[buttonIdx];
        if(controller == 2)
            return GlobalNamespace::OVRInput::Get(button, controllers[0]) || GlobalNamespace::OVRInput::Get(button, controllers[1]);
        return GlobalNamespace::OVRInput::Get(button, controllers[controller]);
    }

    bool SkipBlendShape(AssetLib::Structure::VRM::BlendShapePreset preset, BlendshapeSettings settings)
    {
        if(settings.autoBlink)
        {
            return preset == AssetLib::Structure::VRM::Blink;
        }
        return false;
    }

    void BlendShapeController::Update()
    {
        if(!init) return;
        getLogger().info("blend x1");
        bool any = false; //Used to determine if we should lerp to neutral preset
        auto triggerConfig = Config::ConfigManager::GetControllerTriggerSettings();
        auto blendShapeConfig = Config::ConfigManager::GetBlendShapeSettings();
        getLogger().info("blend x2");

        //Reset All to 0
        for (auto const& [key, val] : blendShapeTargetValues)
        {
            blendShapeTargetValues[key] = 0.0f;
        }
        getLogger().info("blend x3");

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

        getLogger().info("blend x4");

        const auto neutralPreset = reverseMappings[blendShapeConfig.neutralExpression];

        blendShapeTargetValues[neutralPreset] = any ? 0.0f : 1.0f;

        getLogger().info("blend x5");
        //Lerp Values

        for (auto const& [key, val] : blendShapeTargetValues)
        {
            if(SkipBlendShape(key, blendShapeConfig)) continue;
            blendShapeValues[key] = UnityEngine::Mathf::Lerp(blendShapeValues[key], val, UnityEngine::Time::get_deltaTime() * 10.0f);
        }

        getLogger().info("blend x6");

        //Set Values

        for (auto const& [key, val] : blendShapeValues)
        {
            SetBlendshape(key, val * 100.0f);
        }

        getLogger().info("blend x7");

        if(blendShapeConfig.mockEyeMovement)
        {
            if(UnityEngine::Time::get_time() > time)
            {
                //switch eye positions
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
        auto leftAngle = UnityEngine::Quaternion::Lerp(leftEye->get_localRotation(), UnityEngine::Quaternion::Euler(eyeVec), UnityEngine::Time::get_deltaTime() * 15.0f);
        auto rightAngle = UnityEngine::Quaternion::Lerp(leftEye->get_localRotation(), UnityEngine::Quaternion::Euler(eyeVec), UnityEngine::Time::get_deltaTime() * 15.0f);
        leftEye->set_localRotation(leftAngle);
        rightEye->set_localRotation(rightAngle);
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