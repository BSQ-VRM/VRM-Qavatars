#include "sceneEventManager.hpp"

#include <custom-types/shared/delegate.hpp>
#include <UnityEngine/SceneManagement/SceneManager.hpp>

#include "main.hpp"

namespace VRMQavatars
{
    CP_SDK::Utils::Event<> SceneEventManager::OnGameEnter;
    CP_SDK::Utils::Event<> SceneEventManager::OnMenuEnter;

    bool SceneEventManager::inGame;
    bool SceneEventManager::inMenu;

    //I know
    //I know
    //I know
    //Scene manager is bad
    //Open a PR (please)
    void SceneEventManager::Init()
    {
        using namespace UnityEngine::SceneManagement;

        //We may need to remove this delegate but prob not necesary since it persists.
        using type = UnityEngine::Events::UnityAction_2<Scene, LoadSceneMode>*;
        SceneManager::add_sceneLoaded(
            custom_types::MakeDelegate<type>(classof(type), static_cast<std::function<void(Scene, LoadSceneMode)>>([](const Scene scene, LoadSceneMode mode)
            {
                GameSceneChanged(scene);
            }))
        );
    }

    void SceneEventManager::GameSceneChanged(UnityEngine::SceneManagement::Scene scene)
    {
        //Start with ingame being false and check later
        inGame = false;
        const std::string name = scene.get_name();
        getLogger().info("%s", name.c_str());

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

        if((name == "MainMenu" || name == "MenuViewControllers") && inGame)
        {
            //We are in menu
            inMenu = true;
            OnMenuEnter();
            return;
        }
    }
}
