#pragma once

#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"

#include "UnityEngine/SceneManagement/Scene.hpp"

namespace VRMQavatars
{
    class SceneEventManager
    {
    public:
        static void Init();

        static CP_SDK::Utils::Event<> OnGameEnter;
        static CP_SDK::Utils::Event<> OnMenuEnter;

        static bool inGame;
        static bool inMenu;

    private:
        static void GameSceneChanged(UnityEngine::SceneManagement::Scene scene);
    };
}
