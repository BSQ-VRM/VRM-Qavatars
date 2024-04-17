#include "customTypes/PPRender.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp>
#include <sombrero/shared/FastQuaternion.hpp>
#include <sombrero/shared/FastVector3.hpp>

#include "UnityEngine/Graphics.hpp"

namespace VRMQavatars
{
    CP_SDK_IL2CPP_INHERIT_INIT(PPRender);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(PPRender)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(PPRender)
    {

    }

    void PPRender::OnRenderImage(UnityEngine::RenderTexture* src, UnityEngine::RenderTexture* dest)
    {
        UnityEngine::Graphics::Blit(src, dest);
    }

}