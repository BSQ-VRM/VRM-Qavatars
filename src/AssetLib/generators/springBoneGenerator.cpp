#include "AssetLib/generators/springBoneGenerator.hpp"

#include "customTypes/VRMSpringBoneColliderGroup.hpp"
#include "customTypes/VRMSpringBone.hpp"

namespace AssetLib::Generators
{
    Sombrero::FastVector3 convertVector(const VRMC_VRM_0_0::Vector3 vec)
    {
        return {vec.x, vec.y, vec.z};
    }

    template<>
    void SpringBoneGenerator::Generate(const VRMC_VRM_0_0::Vrm& vrm, AssetLib::Structure::VRM::VRMModelContext* modelContext, UnityEngine::GameObject* secondary)
    {
        auto colliders = std::vector<VRMQavatars::VRMSpringBoneColliderGroup*>();
        for (const auto& colliderGroup : vrm.secondaryAnimation.colliderGroups)
        {
            if (auto node = modelContext->nodes[colliderGroup.node+1]->gameObject)
            {
                auto vrmGroup = node->AddComponent<VRMQavatars::VRMSpringBoneColliderGroup*>();
                for (auto colliderRef : colliderGroup.colliders)
                {
                    vrmGroup->colliders.emplace_back(convertVector(colliderRef.offset), colliderRef.radius);
                }
                colliders.push_back(vrmGroup);
            }
            else
            {
                break;
            }
        }

        auto springs = vrm.secondaryAnimation.boneGroups;
        for (auto springRef : springs)
        {
             auto springBone = secondary->get_gameObject()->AddComponent<VRMQavatars::VRMSpringBone*>();

            if (springRef.center != -1)
            {
                springBone->center = modelContext->nodes[springRef.center+1]->gameObject->get_transform();
            }

            springBone->comment = springRef.comment;
            springBone->dragForce = springRef.dragForce;
            springBone->gravityDir = convertVector(springRef.gravityDir);
            springBone->gravityPower = springRef.gravityPower;
            springBone->hitRadius = springRef.hitRadius;
            springBone->stiffnessForce = springRef.stiffiness;
            if ( springRef.colliderGroups.size() > 0)
            {
                springBone->colliderGroups = std::vector<VRMQavatars::VRMSpringBoneColliderGroup*>(springRef.colliderGroups.size());
                for (int j = 0; j < springRef.colliderGroups.size(); j++)
                {
                    auto group = springRef.colliderGroups[j];
                    springBone->colliderGroups[j] = colliders[group];
                }
            }
            auto list2 = ArrayW<UnityEngine::Transform*>(springRef.bones.size());
            for (size_t k = 0; k < springRef.bones.size(); k++)
            {
                auto bone = springRef.bones[k];
                list2[k] = modelContext->nodes[bone+1]->gameObject->get_transform();
            }
            springBone->rootBones = list2;
        }   
    }

    template<>
    void SpringBoneGenerator::Generate(const VRMC_VRM_1_0::Vrm& vrm, AssetLib::Structure::VRM::VRMModelContext* modelContext, UnityEngine::GameObject* secondary)
    {
    }
}