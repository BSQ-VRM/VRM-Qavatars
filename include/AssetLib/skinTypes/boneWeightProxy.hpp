#pragma once
#include "UnityEngine/BoneWeight.hpp"

namespace AssetLib::SkinTypes
{
    struct BoneWeightProxy
    {
        std::vector<int> ids = std::vector<int>(4);
        std::vector<float> weights = {-1.0f, -1.0f, -1.0f, -1.0f};

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
            UnityEngine::BoneWeight bw;

            for (size_t i = 0; i < ids.size(); i++)
            {
                if(i == 0 && weights[i] > 0.0f)
                {
                    bw.set_boneIndex0(ids[i]);
                    bw.set_weight0(std::max(weights[i], 0.0f));
                }
                if(i == 1 && weights[i] > 0.0f)
                {
                    bw.set_boneIndex1(ids[i]);
                    bw.set_weight1(std::max(weights[i], 0.0f));
                }
                if(i == 2 && weights[i] > 0.0f)
                {
                    bw.set_boneIndex2(ids[i]);
                    bw.set_weight2(std::max(weights[i], 0.0f));
                }
                if(i == 3 && weights[i] > 0.0f)
                {
                    bw.set_boneIndex3(ids[i]);
                    bw.set_weight3(std::max(weights[i], 0.0f));
                }
            }
            return bw;
        }
    };
};