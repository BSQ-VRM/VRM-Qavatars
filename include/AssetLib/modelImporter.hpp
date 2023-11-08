#pragma once
#include <string>
#include <fstream>

#include "structure/modelContext.hpp"
#include "structure/VRM/VRMmodelContext.hpp"
#include "mappings/avatarMappings.hpp"
#include "customTypes/TargetManager.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/Rendering/IndexFormat.hpp"
#include "UnityEngine/Matrix4x4.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/TextureFormat.hpp"
#include "UnityEngine/TextureWrapMode.hpp"
#include "UnityEngine/ImageConversion.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Animator.hpp"
#include "UnityEngine/HumanBodyBones.hpp"

#include "RootMotion/FinalIK/VRIK.hpp"
#include "RootMotion/FinalIK/IKSolverVR.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Arm.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Spine.hpp"
#include "RootMotion/FinalIK/VRIK_References.hpp"

#include "assimp/shared/assimp/postprocess.h"
#include "assimp/shared/assimp/Importer.hpp"
#include "assimp/shared/assimp/scene.h"

#include "vrm/vrmIncludes.hpp"
#include "arrayUtils.hpp"
#include "utils/boneMeshUtility.hpp"

#include "customTypes/VRMSpringBone.hpp"

namespace AssetLib
{
    class ModelImporter
    {
        public:
        //Should work with any file type that assimp supports
        //Also works with VRM files but will not parse out springbones and other stuff
        static AssetLib::Structure::ModelContext* Load(const std::string& filename, bool loadMaterials);
        //Use ONLY for vrm files
        static AssetLib::Structure::VRM::VRMModelContext* LoadVRM(const std::string& filename, UnityEngine::Shader* mtoon);
    };
};