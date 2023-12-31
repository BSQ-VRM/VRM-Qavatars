#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Light.hpp"

#include "GlobalNamespace/TubeBloomPrePassLight.hpp"
#include "GlobalNamespace/LightColorBeatmapEventData.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "GlobalNamespace/ColorManager.hpp"

#include "custom-types/shared/coroutine.hpp"

namespace GlobalNamespace
{
	class TubeBloomPrePassLight;
}

namespace VRMQavatars
{
	class LightManager : public UnityEngine::MonoBehaviour
	{
		CP_SDK_IL2CPP_INHERIT("VRMQavatars", LightManager, UnityEngine::MonoBehaviour);
		CP_SDK_IL2CPP_DECLARE_CTOR(LightManager);
		CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(LightManager);
	public:
		//Proxy Funcs
		DECLARE_INSTANCE_METHOD(void, Awake); //Init
		DECLARE_INSTANCE_METHOD(void, Update); //UpdateLights
	public:
		static custom_types::Helpers::Coroutine Grab();

        static void Init();

		static void UpdateMenuLights();
		static void UpdateGameLights();

		static void UpdateLightValues();

		private:
		static bool inGame;

		static SafePtrUnity<UnityEngine::Light> _light;

		//Difference betwen this and global light? not sure but naluluna avatars has it equivalent just with the intensity multiplied by platform light intensity
		static SafePtrUnity<UnityEngine::Light> _platform;

		static std::vector<SafePtrUnity<GlobalNamespace::TubeBloomPrePassLight>> lights;

    };
};

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::LightManager);