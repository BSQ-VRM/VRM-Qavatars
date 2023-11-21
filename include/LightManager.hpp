#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>
#include <UnityEngine/MonoBehaviour.hpp>

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Light.hpp"

#include "GlobalNamespace/LightColorBeatmapEventData.hpp"

namespace VRMQavatars
{
	class LightManager : public UnityEngine::MonoBehaviour
	{
		CP_SDK_IL2CPP_INHERIT("VRMQavatars.OVRLipSync", LightManager, UnityEngine::MonoBehaviour);
		CP_SDK_IL2CPP_DECLARE_CTOR(LightManager);
		CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(LightManager);
	public:
		//Proxy Funcs
		DECLARE_INSTANCE_METHOD(void, Awake); //Init
		DECLARE_INSTANCE_METHOD(void, Update); //UpdateLights
	public:
        static void Init();

		static void UpdateLights();
		static void UpdateMenuLights();
		static void UpdateGameLights();

        static void UpdateLightValues();

		private:
		static bool inGame = false;
		static bool inMenu = false;

		static SafePtrUnity<UnityEngine::Light> _light;
		static SafePtrUnity<UnityEngine::Light> _beatmapLight;

		//Difference betwen this and global light? not sure but naluluna avatars has it equivalent just with the intensity multiplied by platform light intensity
		static SafePtrUnity<UnityEngine::Light> _platform;

		static SafePtrUnity<UnityEngine::Light> _saberLight1;
		static SafePtrUnity<UnityEngine::Light> _saberLight2;

    };
};