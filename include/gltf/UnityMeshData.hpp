#pragma once

#include "main.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/BoneWeight.hpp"
#include "UnityEngine/MeshTopology.hpp"


struct BoneWeightProxy
{
    std::vector<int> ids = std::vector<int>(4);
    std::vector<float> weights = std::vector<float>(4);

    void AddBoneData(uint BoneID, float Weight)
    {
        for (uint i = 0 ; i < 4; i++) {
            if (weights[i] == 0.0) {
                ids[i] = BoneID;
                weights[i] = Weight;
                return;
            }
        }

        // should never get here - more bones than we have space for
        assert(0);
    } 

    UnityEngine::BoneWeight convert()
    {
        UnityEngine::BoneWeight bw;

        for (size_t i = 0; i < ids.size(); i++)
        {
            if(i == 0 && weights[i] > 0.0f)
            {
                bw.set_boneIndex0(ids[i]);
                bw.set_weight0(weights[i]);
            }
            if(i == 1 && weights[i] > 0.0f)
            {
                bw.set_boneIndex1(ids[i]);
                bw.set_weight1(weights[i]);
            }
            if(i == 2 && weights[i] > 0.0f)
            {
                bw.set_boneIndex2(ids[i]);
                bw.set_weight2(weights[i]);
            }
            if(i == 3 && weights[i] > 0.0f)
            {
                bw.set_boneIndex3(ids[i]);
                bw.set_weight3(weights[i]);
            }
        }
        return bw;
    }
};

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
    std::vector<BoneWeightProxy> boneWeights;

    /*std::vector<std::vector<UnityEngine::Vector3>> morphTargetVertices;
    std::vector<std::vector<UnityEngine::Vector3>> morphTargetNormals;
    std::vector<std::vector<UnityEngine::Vector3>> morphTargetTangents;*/

    std::vector<UnityEngine::MeshTopology> topology;
    std::vector<std::vector<int>> indices;
};