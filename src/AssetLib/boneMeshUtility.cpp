#include "AssetLib/boneMeshUtility.hpp"

namespace AssetLib
{
	UnityEngine::Mesh* BoneMeshUtility::CreateErasedMesh(UnityEngine::Mesh* src, const std::vector<int>& eraseBoneIndices)
	{
		UnityEngine::Mesh* mesh = UnityEngine::Mesh::New_ctor();
		mesh->set_name(src->get_name() + "(erased)");
		mesh->set_indexFormat(src->get_indexFormat());
		mesh->set_vertices(src->get_vertices());
		mesh->set_normals(src->get_normals());
		mesh->set_uv(src->get_uv());
		mesh->set_tangents(src->get_tangents());
		mesh->set_boneWeights(src->get_boneWeights());
		mesh->set_bindposes(src->get_bindposes());
		mesh->set_subMeshCount(src->get_subMeshCount());
		for (int i = 0; i < src->get_subMeshCount(); i++)
		{
			ArrayW<int> indices = src->GetIndices(i);
			const int num = ExcludeTriangles(indices, src->get_boneWeights(), eraseBoneIndices);
			ArrayW<int> array = ArrayW<int>(num);
			for (int j = 0; j < array.size(); j++)
			{
				array[j] = indices[j];
			}
			mesh->SetIndices(array, UnityEngine::MeshTopology::Triangles, i);
		}
		return mesh;
	}

	int BoneMeshUtility::ExcludeTriangles(ArrayW<int>& triangles, ArrayW<UnityEngine::BoneWeight> bws, const std::vector<int>& exclude)
	{
		int num = 0;
		if(bws.size() > 0)
		{
			for (int i = 0; i < triangles.size(); i += 3)
			{
				const int num2 = triangles[i];
				const int num3 = triangles[i + 1];
				const int num4 = triangles[i + 2];
				const UnityEngine::BoneWeight boneWeight = bws[num2];
				const ExcludeBoneIndex excludeBoneIndex = AreBoneContains(exclude, boneWeight.m_BoneIndex0, boneWeight.m_BoneIndex1, boneWeight.m_BoneIndex2, boneWeight.m_BoneIndex3);
				if ((boneWeight.m_Weight0 <= 0.0f || !excludeBoneIndex.Bone0) && (boneWeight.m_Weight1 <= 0.0f || !excludeBoneIndex.Bone1) && (boneWeight.m_Weight2 <= 0.0f || !excludeBoneIndex.Bone2) && (boneWeight.m_Weight3 <= 0.0f || !excludeBoneIndex.Bone3))
				{
					const UnityEngine::BoneWeight boneWeight2 = bws[num3];
					const ExcludeBoneIndex excludeBoneIndex2 = AreBoneContains(exclude, boneWeight2.m_BoneIndex0, boneWeight2.m_BoneIndex1, boneWeight2.m_BoneIndex2, boneWeight2.m_BoneIndex3);
					if ((boneWeight2.m_Weight0 <= 0.0f || !excludeBoneIndex2.Bone0) && (boneWeight2.m_Weight1 <= 0.0f || !excludeBoneIndex2.Bone1) && (boneWeight2.m_Weight2 <= 0.0f || !excludeBoneIndex2.Bone2) && (boneWeight2.m_Weight3 <= 0.0f || !excludeBoneIndex2.Bone3))
					{
						const UnityEngine::BoneWeight boneWeight3 = bws[num4];
						const ExcludeBoneIndex excludeBoneIndex3 = AreBoneContains(exclude, boneWeight3.m_BoneIndex0, boneWeight3.m_BoneIndex1, boneWeight3.m_BoneIndex2, boneWeight3.m_BoneIndex3);
						if ((boneWeight3.m_Weight0 <= 0.0f || !excludeBoneIndex3.Bone0) && (boneWeight3.m_Weight1 <= 0.0f || !excludeBoneIndex3.Bone1) && (boneWeight3.m_Weight2 <= 0.0f || !excludeBoneIndex3.Bone2) && (boneWeight3.m_Weight3 <= 0.0f || !excludeBoneIndex3.Bone3))
						{
							triangles[num++] = num2;
							triangles[num++] = num3;
							triangles[num++] = num4;
						}
					}
				}
			}
		}
		return num;
	}

	BoneMeshUtility::ExcludeBoneIndex BoneMeshUtility::AreBoneContains(const std::vector<int>& exclude, const int boneIndex0, const int boneIndex1, const int boneIndex2, const int boneIndex3)
	{
		bool flag = false;
		bool flag2 = false;
		bool flag3 = false;
		bool flag4 = false;
		for (int i = 0; i < exclude.size(); i++)
		{
			if (exclude[i] == boneIndex0)
			{
				flag = true;
			}
			else if (exclude[i] == boneIndex1)
			{
				flag2 = true;
			}
			else if (exclude[i] == boneIndex2)
			{
				flag3 = true;
			}
			else if (exclude[i] == boneIndex3)
			{
				flag4 = true;
			}
		}
		return ExcludeBoneIndex(flag, flag2, flag3, flag4);
	}
}