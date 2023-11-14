#pragma once

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Light.hpp"

namespace VRMQavatars
{
    class LightManager
    {
        public:
        static void Init();
        static void SetGlobalLightIntensity(float val);
        static void SetGlobalLightColor(UnityEngine::Color val);
        static void SetGlobalLightRotation(UnityEngine::Vector3 val);
		private:
		static SafePtrUnity<UnityEngine::Light> _light;
    };
};