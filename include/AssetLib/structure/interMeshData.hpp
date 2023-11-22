#pragma once
#include "main.hpp"
#include <map>

#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/BoneWeight.hpp"
#include "UnityEngine/MeshTopology.hpp"

#include "AssetLib/skinTypes/boneWeightProxy.hpp"

namespace AssetLib::Structure
{
    struct BlendShapeInfo
    {
        std::vector<UnityEngine::Vector3> vertices;
        std::vector<UnityEngine::Vector3> normals;
        std::vector<UnityEngine::Vector3> tangents;
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
        std::vector<UnityEngine::Vector3> vertices;
        std::vector<UnityEngine::Vector3> normals;
        std::vector<UnityEngine::Vector4> tangents;
        std::vector<UnityEngine::Vector2> uv1;
        std::vector<UnityEngine::Vector2> uv2;
        std::vector<UnityEngine::Vector2> uv3;
        std::vector<UnityEngine::Vector2> uv4;
        std::vector<UnityEngine::Color> colors;
        std::vector<AssetLib::SkinTypes::BoneWeightProxy> boneWeights;

        std::vector<int> vertexCounts;
        std::vector<int> materialIdxs;

        //Blend Shapes
        std::vector<std::string> morphTargetNames = {};
        std::vector<BlendShapeInfo> morphTargetInfos = {};

        std::vector<UnityEngine::MeshTopology> topology;
        std::vector<std::vector<int>> indices;
    };
};