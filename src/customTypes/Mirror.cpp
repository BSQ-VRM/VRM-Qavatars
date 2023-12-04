#include "customTypes/Mirror.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp>
#include <UnityEngine/Camera.hpp>

#include "SceneEventManager.hpp"
#include "config/ConfigManager.hpp"

namespace VRMQavatars
{
    CP_SDK_IL2CPP_INHERIT_INIT(Mirror);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(Mirror)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(Mirror)
    {

    }

    void Mirror::Update()
    {
        if(camera == nullptr)
        {
            camera = GetComponentInChildren<UnityEngine::Camera*>()->get_transform();
        }

        const auto mirrorSettings = Config::ConfigManager::GetMirrorSettings();

        if(lastTrack != mirrorSettings.boneTracking || !target)
        {
            if(mirrorSettings.boneTracking != 0)
            {
                const UnityEngine::HumanBodyBones target = mirrorSettings.boneTracking == 1 ? UnityEngine::HumanBodyBones::Hips : UnityEngine::HumanBodyBones::Head;
                if(AvatarManager::currentContext != nullptr)
                {
                    this->target = AvatarManager::currentContext->rootGameObject->GetComponent<UnityEngine::Animator*>()->GetBoneTransform(target);
                }
            }
            else
            {
                target = nullptr;
            }
            lastTrack = mirrorSettings.boneTracking;
        }

        if(target)
        {
            //Head bone is placed more at the neck
            const auto newPosition = target->get_position() + (target->get_up()*0.1f);
            camera->set_position(newPosition + (target->get_forward() * mirrorSettings.distance));
            camera->LookAt(newPosition);
        }
        else
        {
            camera->set_localPosition({0,0,-1});
            camera->set_localRotation({0,0,0});
        }

        const bool shouldShowMenu = SceneEventManager::inMenu && (mirrorSettings.scene == 1);
        const bool shouldShowGame = SceneEventManager::inGame && (mirrorSettings.scene == 2);
        const bool shouldShow = shouldShowMenu || shouldShowGame || mirrorSettings.scene == 0;
        get_transform()->get_parent()->get_gameObject()->SetActive(shouldShow && mirrorSettings.enabled);
    }
    void Mirror::Start()
    {
        SceneEventManager::OnMenuEnter += [this]
        {
            const auto mirrorSettings = Config::ConfigManager::GetMirrorSettings();
            const bool shouldShowMenu = mirrorSettings.scene == 1;
            const bool shouldShow = shouldShowMenu || mirrorSettings.scene == 0;
            get_transform()->get_parent()->get_gameObject()->SetActive(shouldShow);
        };

        SceneEventManager::OnGameEnter += [this]
        {
            const auto mirrorSettings = Config::ConfigManager::GetMirrorSettings();
            const bool shouldShowGame = mirrorSettings.scene == 2;
            const bool shouldShow = shouldShowGame || mirrorSettings.scene == 0;
            get_transform()->get_parent()->get_gameObject()->SetActive(shouldShow);
        };
    }

}