#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>
#include <sombrero/shared/FastVector3.hpp>

#include "main.hpp"

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Material.hpp"

#include "custom-types/shared/delegate.hpp"

namespace VRMQavatars
{
	class PPRender : public UnityEngine::MonoBehaviour
	{
		CP_SDK_IL2CPP_INHERIT("VRMQavatars", PPRender, UnityEngine::MonoBehaviour);
		CP_SDK_IL2CPP_DECLARE_CTOR(PPRender);
		CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(PPRender);

		DECLARE_INSTANCE_METHOD(void, OnRenderImage, UnityEngine::RenderTexture* src, UnityEngine::RenderTexture* dest);
		DECLARE_INSTANCE_FIELD(UnityEngine::Material*, toBlit);
	};
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::PPRender);