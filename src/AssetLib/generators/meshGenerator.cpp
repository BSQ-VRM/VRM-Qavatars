#include "AssetLib/generators/meshGenerator.hpp"

#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Matrix4x4.hpp"

#include "AssetLib/ArrayUtils.hpp"

namespace AssetLib::Generators
{
    void MeshGenerator::Generate(const AssetLib::Structure::Node* node, const AssetLib::Structure::ModelContext* context)
    {
        if(node->mesh.has_value())
        {
            auto mesh = node->mesh.value();
        
            UnityEngine::Mesh* unityMesh = UnityEngine::Mesh::New_ctor();
            unityMesh->set_name(node->name);
            unityMesh->set_indexFormat(mesh.vertices.size() > 65535 ? UnityEngine::Rendering::IndexFormat::UInt32 : UnityEngine::Rendering::IndexFormat::UInt16);

            unityMesh->set_vertices(il2cpp_utils::vectorToArray(mesh.vertices));
            unityMesh->set_normals(il2cpp_utils::vectorToArray(mesh.normals));
            unityMesh->set_tangents(il2cpp_utils::vectorToArray(mesh.tangents));
            unityMesh->set_uv(il2cpp_utils::vectorToArray(mesh.uv1));
            unityMesh->set_uv2(il2cpp_utils::vectorToArray(mesh.uv2));
            unityMesh->set_uv3(il2cpp_utils::vectorToArray(mesh.uv3));
            unityMesh->set_uv4(il2cpp_utils::vectorToArray(mesh.uv4));
            unityMesh->set_colors(il2cpp_utils::vectorToArray(mesh.colors));
            
            std::vector<UnityEngine::BoneWeight> convertedBW = std::vector<UnityEngine::BoneWeight>(mesh.boneWeights.size());

            for (size_t i = 0; i < mesh.boneWeights.size(); i++)
            {
                convertedBW[i] = mesh.boneWeights[i].convert();
            }
            
            unityMesh->set_boneWeights(il2cpp_utils::vectorToArray(convertedBW));
            unityMesh->set_subMeshCount(mesh.indices.size());
            uint baseVertex = 0;
            for (int i = 0; i < mesh.indices.size(); i++)
            {
                const ArrayW<int> indices = il2cpp_utils::vectorToArray<int>(mesh.indices[i]);
                unityMesh->SetIndices(indices, mesh.topology[i], i, false, static_cast<int>(baseVertex));
                baseVertex += mesh.vertexCounts[i];
            }
            unityMesh->RecalculateBounds();

            for (int i = 0; i < mesh.morphTargetNames.size(); ++i)
            {
                auto name = mesh.morphTargetNames[i];
                auto [vertices, normals, tangents] = mesh.morphTargetInfos[i];
                if(unityMesh->GetBlendShapeIndex(name) == -1 && !System::String::IsNullOrWhiteSpace(name))
                {
                    unityMesh->AddBlendShapeFrame(name, 100,
                        il2cpp_utils::vectorToArray(vertices),
                        il2cpp_utils::vectorToArray(normals),
                        nullptr
                    );
                }
            }

            if(context->isSkinned)
            {
                const auto renderer = node->gameObject->AddComponent<UnityEngine::SkinnedMeshRenderer*>();
                renderer->set_updateWhenOffscreen(true);
                renderer->set_allowOcclusionWhenDynamic(false);
                const auto armature = context->armature.value();

                std::vector<UnityEngine::Matrix4x4> bindPoses = std::vector<UnityEngine::Matrix4x4>(context->nodes.size());

                for (size_t i = 0; i < context->nodes.size(); i++)
                {
                    const auto ctxNode = context->nodes[i];
                    if(ctxNode->isBone)
                    {
                        bindPoses[i] = UnityEngine::Matrix4x4::op_Multiply(ctxNode->gameObject->get_transform()->get_worldToLocalMatrix(), renderer->get_transform()->get_localToWorldMatrix());
                    }
                } 

                unityMesh->set_bindposes(il2cpp_utils::vectorToArray(bindPoses));

                renderer->set_rootBone(armature.rootBone->gameObject->get_transform());

                auto bones = ArrayUtils::Select<UnityEngine::Transform*>(context->nodes,
                                        [](const AssetLib::Structure::Node* libNode){
                                            return libNode->gameObject->get_transform();
                                        }
                                    );
                renderer->set_bones(il2cpp_utils::vectorToArray(bones));
                renderer->set_sharedMesh(unityMesh);
                unityMesh->RecalculateBounds();
            }
            else
            {
                const auto filter = node->gameObject->AddComponent<UnityEngine::MeshFilter*>();
                node->gameObject->AddComponent<UnityEngine::MeshRenderer*>();
                filter->set_sharedMesh(unityMesh);
            }
        }
    }
}