#pragma once

#include "main.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/BoneWeight.hpp"
#include "UnityEngine/MeshTopology.hpp"

//Based off https://github.com/KhronosGroup/UnityGLTF/blob/master/UnityGLTF/Assets/UnityGLTF/Runtime/Scripts/GLTFSceneImporter.cs#L41
struct UnityMeshData
{
    std::vector<UnityEngine::Vector3> vertices;
    std::vector<UnityEngine::Vector3> normals;
    std::vector<UnityEngine::Vector4> tangents;
    std::vector<UnityEngine::Vector2> uv1;
    std::vector<UnityEngine::Vector2> uv2;
    std::vector<UnityEngine::Vector2> uv3;
    std::vector<UnityEngine::Vector2> uv4;
    std::vector<UnityEngine::Color> colors;
    std::vector<UnityEngine::BoneWeight> boneWeights;

    std::vector<std::vector<UnityEngine::Vector3>> morphTargetVertices;
    std::vector<std::vector<UnityEngine::Vector3>> morphTargetNormals;
    std::vector<std::vector<UnityEngine::Vector3>> morphTargetTangents;

    std::vector<UnityEngine::MeshTopology> topology;
    std::vector<std::vector<int>> indices;
};