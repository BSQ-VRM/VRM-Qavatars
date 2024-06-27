#pragma once
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/BoneWeight.hpp"
#include "UnityEngine/Rendering/IndexFormat.hpp"

#include "AssetLib/arrayUtils.hpp"

namespace AssetLib {
    class BoneMeshUtility
    {
        public:
        struct ExcludeBoneIndex
		{
			ExcludeBoneIndex(bool bone0, bool bone1, bool bone2, bool bone3)
			{
				this->Bone0 = bone0;
				this->Bone1 = bone1;
				this->Bone2 = bone2;
				this->Bone3 = bone3;
			}

			bool Bone0;
			bool Bone1;
			bool Bone2;
			bool Bone3;
		};

        static UnityEngine::Mesh* CreateErasedMesh(UnityEngine::Mesh* src, const std::vector<int>& eraseBoneIndices);
        static int ExcludeTriangles(ArrayW<int>& triangles, ArrayW<UnityEngine::BoneWeight> bws, const std::vector<int>& exclude);
        static ExcludeBoneIndex AreBoneContains(const std::vector<int>& exclude, int boneIndex0, int boneIndex1, int boneIndex2, int boneIndex3);
    };
};