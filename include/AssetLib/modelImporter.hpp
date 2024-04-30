#pragma once
#include <string>
#include <fstream>

#include "structure/modelContext.hpp"
#include "structure/VRM/VRMmodelContext.hpp"
#include "customTypes/TargetManager.hpp"

#include "UnityEngine/Shader.hpp"

namespace AssetLib
{
    class ModelImporter
    {
        public:
        //Should work with any file type that assimp supports
        //Also works with VRM files but will not parse out springbones and other stuff
        static void Load(const std::string& filename, bool loadMaterials, const std::function<void(Structure::ModelContext*)>& onFinish);
        //Use ONLY for vrm files
        static void LoadVRM(const std::string& filename, const std::function<void(Structure::VRM::VRMModelContext*)>& onFinish);

        static SafePtrUnity<UnityEngine::Shader> mtoon;
    };
};