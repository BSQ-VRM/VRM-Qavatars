#pragma once
#include <string>

#include "structure/modelContext.hpp"
#include "structure/VRM/VRMmodelContext.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Transform.hpp"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "vrmIncludes.hpp"

namespace AssetLib
{
    class ModelImporter
    {
        public:
        //Should work with any file type that assimp supports
        //Also works with VRM files but will not parse out springbones and other stuff
        static AssetLib::Structure::ModelContext* Load(const std::string& filename);
        //Use ONLY for vrm files
        static AssetLib::Structure::VRM::VRMModelContext* LoadVRM(const std::string& filename);
    };
};