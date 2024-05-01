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
        static std::future<Structure::ModelContext*> Load(const std::string& filename, bool loadMaterials);
        //Use ONLY for vrm files
        static std::future<Structure::VRM::VRMModelContext*> LoadVRM(const std::string& filename);

        static SafePtrUnity<UnityEngine::Shader> mtoon;
    };
};