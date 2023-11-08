#include "AvatarManager.hpp"

AssetLib::Structure::VRM::VRMModelContext* VRMQavatars::AvatarManager::currentContext;

void VRMQavatars::AvatarManager::SetContext(AssetLib::Structure::VRM::VRMModelContext* context)
{
    if(currentContext != nullptr)
    {
        UnityEngine::GameObject::Destroy(currentContext->rootGameObject);
        delete currentContext;
    }
    currentContext = context;
}