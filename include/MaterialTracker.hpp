#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Material.hpp"

namespace VRMQavatars
{
    class MaterialTracker
    {
        public:
        static void UpdateMaterials();
        static bool bloomEnabled;
        static std::vector<SafePtrUnity<UnityEngine::Material>> materials;
    };
}
