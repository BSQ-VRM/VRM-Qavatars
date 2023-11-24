#include "GroundOffsetManager.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/Unity/MTCoroutineStarter.hpp>
#include <custom-types/shared/coroutine.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/WaitForSeconds.hpp>

#include "sceneEventManager.hpp"
#include "customTypes/GroundOffsetObject.hpp"

namespace VRMQavatars
{
    void GroundOffsetManager::Init()
    {
        SceneEventManager::OnGameEnter += { &OnGameEnter };
        SceneEventManager::OnMenuEnter += { &OnMenuEnter };
        OnMenuEnter();
    }

    custom_types::Helpers::Coroutine Wait(const std::string toAdd)
    {
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(UnityEngine::WaitForSeconds::New_ctor(0.2f)));
        UnityEngine::GameObject::Find(toAdd)->AddComponent<GroundOffsetObject*>();
        co_return;
    }

    void GroundOffsetManager::OnMenuEnter()
    {
        CP_SDK::Unity::MTCoroutineStarter::Start(custom_types::Helpers::CoroutineHelper::New(Wait("DefaultMenuEnvironment")));
    }

    void GroundOffsetManager::OnGameEnter()
    {
        CP_SDK::Unity::MTCoroutineStarter::Start(custom_types::Helpers::CoroutineHelper::New(Wait("PlayersPlace")));
    }
}
