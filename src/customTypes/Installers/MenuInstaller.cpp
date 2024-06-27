#include "customTypes/Installers/MenuInstaller.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/Resources.hpp"
#include "LightManager.hpp"
#include "VMC/VMCClient.hpp"
#include "VMC/VMCServer.hpp"
#include "GroundOffsetManager.hpp"
#include "MirrorManager.hpp"
#include "AvatarManager.hpp"
#include "AssetLib/shaders/shaderLoader.hpp"
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"

DEFINE_TYPE(VRMQavatars, MenuInstaller);

namespace VRMQavatars
{
    custom_types::Helpers::Coroutine Setup() {    
        if(UnityEngine::Resources::FindObjectsOfTypeAll<VRMQavatars::LightManager*>().size() == 0)
        {
            auto lightManager = UnityEngine::GameObject::New_ctor("LightManager")->AddComponent<VRMQavatars::LightManager*>();
        }

        VRMQavatars::VMC::VMCClient::InitClient();
        VRMQavatars::VMC::VMCServer::InitServer();
        VRMQavatars::GroundOffsetManager::Init();

        for(int i = 0; i < 10; i++)
        {
            co_yield nullptr;
        }

        if(!AssetLib::ShaderLoader::shaders)
        {
            co_yield custom_types::Helpers::CoroutineHelper::New(AssetLib::ShaderLoader::LoadBund());
        }

        VRMQavatars::MirrorManager::CreateMainMirror();

        if(VRMQavatars::AvatarManager::currentContext == nullptr)
        {
            VRMQavatars::AvatarManager::StartupLoad();
        }
        co_return;
    }

    void MenuInstaller::InstallBindings()
    {
        BSML::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(Setup()));
    }
}