#pragma once
#include "main.hpp"
#include <map>

#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/BoneWeight.hpp"
#include "UnityEngine/MeshTopology.hpp"

#include "AssetLib/structure/boneWeightProxy.hpp"

#include "sombrero/shared/FastVector3.hpp"
#include "sombrero/shared/FastVector2.hpp"
#include "sombrero/shared/FastColor.hpp"

namespace AssetLib::Structure
{
    struct BlendShapeInfo
    {
        std::vector<Sombrero::FastVector3> vertices;
        std::vector<Sombrero::FastVector3> normals;
        std::vector<Sombrero::FastVector3> tangents;
    };
    //Based off https://github.com/KhronosGroup/UnityGLTF/blob/master/UnityGLTF/Assets/UnityGLTF/Runtime/Scripts/GLTFSceneImporter.cs#L41
    //Intermediate format containing mesh data
    //Assimp -> 
    //  InterMeshData -> 
    //      UnityEngine::Mesh*, 
    //      (UnityEngine::MeshFilter*), 
    //      UnityEngine::(Skinned)MeshRenderer*
    struct InterMeshData
    {
        std::vector<Sombrero::FastVector3> vertices;
        std::vector<Sombrero::FastVector3> normals;
        std::vector<UnityEngine::Vector4> tangents;
        std::vector<Sombrero::FastVector2> uv1;
        std::vector<Sombrero::FastVector2> uv2;
        std::vector<Sombrero::FastVector2> uv3;
        std::vector<Sombrero::FastVector2> uv4;
        std::vector<Sombrero::FastColor> colors;
        std::vector<SkinTypes::BoneWeightProxy> boneWeights;

        std::vector<int> vertexCounts;
        std::vector<int> materialIdxs;

        //Blend Shapes
        std::vector<std::string> morphTargetNames = {};
        std::vector<BlendShapeInfo> morphTargetInfos = {};

        std::vector<UnityEngine::MeshTopology> topology;
        std::vector<std::vector<int>> indices;
    };
};