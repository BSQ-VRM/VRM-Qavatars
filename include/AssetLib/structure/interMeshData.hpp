#pragma once
#include "main.hpp"

#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/BoneWeight.hpp"
#include "UnityEngine/MeshTopology.hpp"

#include "AssetLib/skinTypes/boneWeightProxy.hpp"

namespace AssetLib::Structure
{
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

        //Blend Shapes
        /*std::vector<std::vector<UnityEngine::Vector3>> morphTargetVertices;
        std::vector<std::vector<UnityEngine::Vector3>> morphTargetNormals;
        std::vector<std::vector<UnityEngine::Vector3>> morphTargetTangents;*/

        std::vector<UnityEngine::MeshTopology> topology;
        std::vector<std::vector<int>> indices;
    };
};