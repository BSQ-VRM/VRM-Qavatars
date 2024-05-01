#include "AssetLib/generators/intermediateMeshGenerator.hpp"

namespace AssetLib::Generators
{
    //TODO: find something better lmao
    int IndexForName(const char* name, const AssetLib::Structure::ModelContext* context)
    {
        for (size_t i = 0; i < context->nodes.size(); i++)
        {
            if(context->nodes[i]->name == name)
            {
                return i;
            }
        }
        return -1;
    }

    AssetLib::Structure::InterMeshData IntermediateMeshGenerator::Generate(aiMesh* mesh, AssetLib::Structure::ModelContext* context, std::optional<AssetLib::Structure::InterMeshData> existingMesh)
    {
        auto meshData = existingMesh.has_value() ? existingMesh.value() : AssetLib::Structure::InterMeshData();
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto vert = mesh->mVertices[i];
            meshData.vertices.push_back(Sombrero::FastVector3(vert.x, vert.y, vert.z));
        }
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto norm = mesh->mNormals[i];
            meshData.normals.push_back(Sombrero::FastVector3(norm.x, norm.y, norm.z));
        }
        if (mesh->mTangents != nullptr)
        {
            for (size_t i = 0; i < mesh->mNumVertices; i++)
            {
                auto tang = mesh->mTangents[i];
                meshData.tangents.push_back(UnityEngine::Vector4(tang.x, tang.y, tang.z, 0.0f));
            }
        }
        if (mesh->mTextureCoords[0] != nullptr)
        {
            for (size_t i = 0; i < mesh->mNumVertices; i++)
            {
                auto tex = mesh->mTextureCoords[0][i];
                meshData.uv1.push_back(Sombrero::FastVector2(tex.x, tex.y));
            }
        }
        if (mesh->mTextureCoords[1] != nullptr)
        {
            for (size_t i = 0; i < mesh->mNumVertices; i++)
            {
                auto tex = mesh->mTextureCoords[1][i];
                meshData.uv2.push_back(Sombrero::FastVector2(tex.x, tex.y));
            }
        }
        if (mesh->mTextureCoords[2] != nullptr)
        {
            for (size_t i = 0; i < mesh->mNumVertices; i++)
            {
                auto tex = mesh->mTextureCoords[2][i];
                meshData.uv3.push_back(Sombrero::FastVector2(tex.x, tex.y));
            }
        }
        if (mesh->mTextureCoords[3] != nullptr)
        {
            for (size_t i = 0; i < mesh->mNumVertices; i++)
            {
                auto tex = mesh->mTextureCoords[3][i];
                meshData.uv4.push_back(Sombrero::FastVector2(tex.x, tex.y));
            }
        }
        if (mesh->mColors[0] != nullptr)
        {
            for (size_t i = 0; i < mesh->mNumVertices; i++)
            {
                auto col = mesh->mColors[0][i];
                meshData.colors.push_back(Sombrero::FastColor(col.r, col.g, col.b, col.a));
            }
        }

        meshData.vertexCounts.push_back(mesh->mNumVertices);
        meshData.materialIdxs.push_back(mesh->mMaterialIndex);

        meshData.indices.push_back(std::vector<int>(0));
        meshData.topology.push_back(UnityEngine::MeshTopology::Triangles); //TODO

        if (mesh->mFaces != nullptr)
        {
            for (size_t i = 0; i < mesh->mNumFaces; i++)
            {
                auto face = mesh->mFaces[i];
                for (size_t x = 2; x < face.mNumIndices; x--)
                {
                    meshData.indices[meshData.indices.size()-1].emplace_back(face.mIndices[x]);
                }
            }
        }

        if(context->isSkinned)
        {
            int start = 0;
            if(meshData.boneWeights.size() == 0)
            {
                meshData.boneWeights = std::vector<AssetLib::SkinTypes::BoneWeightProxy>(mesh->mNumVertices);
            }
            else
            {
                start = meshData.boneWeights.size();
                meshData.boneWeights.resize(start + mesh->mNumVertices);
            }

            //i: bone
            //j: current vertexWeight in bone
            //k: weight idx to check in vertex

            //Iterate over every bone in this mesh
            for (int i = 0; i < mesh->mNumBones; i++)
            {
                aiBone* aiBone = mesh->mBones[i];
                
                //Iterate over every vertex that this bone affects
                for (int j = 0; j < aiBone->mNumWeights; j++)
                {
                    aiVertexWeight weight = aiBone->mWeights[j];

                    uint VertexID = start + weight.mVertexId;

                    float Weight = weight.mWeight;

                    meshData.boneWeights[VertexID].AddBoneData(IndexForName(aiBone->mName.C_Str(), context), Weight);
                }
            }
        }
        for (size_t k = 0; k < mesh->mNumAnimMeshes; k++)
        {
            auto animMesh = mesh->mAnimMeshes[k];

            std::string name = animMesh->mName.C_Str();
            
            auto names = meshData.morphTargetNames;
            auto find = std::find(names.begin(), names.end(), name);

            int index;

            if(find == names.end())
            {
                meshData.morphTargetNames.push_back(name);
                meshData.morphTargetInfos.push_back({});
                index = meshData.morphTargetNames.size()-1;
            }
            else
            {
                index = find - names.begin();
            }

            for (size_t i = 0; i < animMesh->mNumVertices; i++)
            {
                auto vert = animMesh->mVertices[i];
                auto ogVert = mesh->mVertices[i];
                meshData.morphTargetInfos[index].vertices.push_back(Sombrero::FastVector3(vert.x - ogVert.x, vert.y - ogVert.y, vert.z - ogVert.z));
            }

            if (animMesh->mNormals != nullptr)
            {
                for (size_t i = 0; i < animMesh->mNumVertices; i++)
                {
                    auto norm = animMesh->mNormals[i];
                    auto ogNorm = mesh->mNormals[i];
                    meshData.morphTargetInfos[index].normals.push_back(Sombrero::FastVector3(norm.x - ogNorm.x, norm.y - ogNorm.y, norm.z - ogNorm.z));
                }
            }

            if (animMesh->mTangents != nullptr)
            {
                for (size_t i = 0; i < animMesh->mNumVertices; i++)
                {
                    auto tang = animMesh->mTangents[i];
                    meshData.morphTargetInfos[index].tangents.push_back(Sombrero::FastVector3(tang.x, tang.y, tang.z));
                }
            }
        }

        return meshData;
    }
}