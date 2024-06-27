#include "SceneEventManager.hpp"

#include <custom-types/shared/delegate.hpp>
#include <UnityEngine/SceneManagement/SceneManager.hpp>

#include "main.hpp"

namespace VRMQavatars
{
    CP_SDK::Utils::Event<> SceneEventManager::OnGameEnter;
    CP_SDK::Utils::Event<> SceneEventManager::OnMenuEnter;

    bool SceneEventManager::inGame;
    bool SceneEventManager::inMenu;

    void SceneEventManager::GameSceneChanged(UnityEngine::SceneManagement::Scene scene)
    {
        //Start with ingame being false and check later
        inGame = false;
        const std::string name = scene.get_name();
        VRMLogger.info("{}", name.c_str());

        if(name == "QuestInit" || name == "ShaderWarmup")
        {
            //We are not in the menu yet and we are not ingame
            inMenu = false;
        }

        if(name == "GameCore")
        {
            //We are ingame
            inGame = true;
            inMenu = false; //We are not in menu anymore
            OnGameEnter();
            return;
        }

        if((name == "MainMenu" || name == "MenuViewControllers") && !inMenu)
        {
            //We are in menu
            inMenu = true;
            OnMenuEnter();
        }
    }
}
