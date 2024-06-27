#pragma once
#include "UnityEngine/BoneWeight.hpp"

namespace AssetLib::SkinTypes
{
    struct BoneWeightProxy
    {
        std::array<int, 4> ids = {0, 0, 0, 0};
        std::array<float, 4> weights = {-1.0f, -1.0f, -1.0f, -1.0f};

        void AddBoneData(uint BoneID, float Weight)
        {
            for (uint i = 0 ; i < 4; i++) {
                if (weights[i] < 0.0) {
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
            return UnityEngine::BoneWeight(
                std::max(weights[0], 0.0f),
                std::max(weights[1], 0.0f),
                std::max(weights[2], 0.0f),
                std::max(weights[3], 0.0f),
                weights[0] > 0.0f ? ids[0] : 0,
                weights[1] > 0.0f ? ids[1] : 0,
                weights[2] > 0.0f ? ids[2] : 0,
                weights[3] > 0.0f ? ids[3] : 0
            );
        }
    };
};