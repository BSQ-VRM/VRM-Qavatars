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
        getLogger().info("x3");
        //Start with ingame being false and check later
        inGame = false;
        getLogger().info("x4");
        const std::string name = scene.get_name();
        getLogger().info("x5");
        getLogger().info("%s", name.c_str());

        if(name == "QuestInit" || name == "ShaderWarmup")
        {
            getLogger().info("x6");
            //We are not in the menu yet and we are not ingame
            inMenu = false;
        }

        if(name == "GameCore")
        {
            getLogger().info("x7");
            //We are ingame
            inGame = true;
            inMenu = false; //We are not in menu anymore
            OnGameEnter();
            return;
        }

        if((name == "MainMenu" || name == "MenuViewControllers") && !inMenu)
        {
            getLogger().info("x8");
            //We are in menu
            inMenu = true;
            OnMenuEnter();
        }
    }
}
