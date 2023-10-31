#pragma once

#define USE_VRMC_VRM_0_0
#include "../vrm.h"

#include "main.hpp"
#include "arrayUtils.hpp"

#include "UnityEngine/Avatar.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SkeletonBone.hpp"
#include "UnityEngine/HumanBone.hpp"
#include "UnityEngine/HumanLimit.hpp"
#include "UnityEngine/HumanDescription.hpp"

namespace VRM::Mappings
{
    class AvatarMappings
    {
        public:

        static UnityEngine::Vector3 convertVector(VRMC_VRM_0_0::Vector3 vec)
        {
            return UnityEngine::Vector3(vec.x, vec.y, vec.z);
        }
        static UnityEngine::Avatar* CreateAvatar(VRMC_VRM_0_0::Vrm vrm, std::vector<UnityEngine::Transform*> bones, UnityEngine::GameObject* root)
        {
            auto humanoid = vrm.humanoid;
            auto hd = UnityEngine::HumanDescription();

            static const std::vector<std::string> names = {
                "Hips",
                "LeftUpperLeg",
                "RightUpperLeg",
                "LeftLowerLeg",
                "RightLowerLeg",
                "LeftFoot",
                "RightFoot",
                "Spine",
                "Chest",
                "Neck",
                "Head",
                "LeftShoulder",
                "RightShoulder",
                "LeftUpperArm",
                "RightUpperArm",
                "LeftLowerArm",
                "RightLowerArm",
                "LeftHand",
                "RightHand",
                "LeftToes",
                "RightToes",
                "LeftEye",
                "RightEye",
                "Left Thumb Proximal",
                "Left Thumb Intermediate",
                "Left Thumb Distal",
                "Left Index Proximal",
                "Left Index Intermediate",
                "Left Index Distal",
                "Left Middle Proximal",
                "Left Middle Intermediate",
                "Left Middle Distal",
                "Left Ring Proximal",
                "Left Ring Intermediate",
                "Left Ring Distal",
                "Left Little Proximal",
                "Left Little Intermediate",
                "Left Little Distal",
                "Right Thumb Proximal",
                "Right Thumb Intermediate",
                "Right Thumb Distal",
                "Right Index Proximal",
                "Right Index Intermediate",
                "Right Index Distal",
                "Right Middle Proximal",
                "Right Middle Intermediate",
                "Right Middle Distal",
                "Right Ring Proximal",
                "Right Ring Intermediate",
                "Right Ring Distal",
                "Right Little Proximal",
                "Right Little Intermediate",
                "Right Little Distal",
                "UpperChest",
            };

            ArrayW<UnityEngine::HumanBone> humanBones(humanoid.humanBones.size());

            for (size_t i = 0; i < humanoid.humanBones.size(); i++)
            {
                getLogger().info("%lu", i);
                auto& bone = humanoid.humanBones[i];
                StringW boneName = bones[bone.node]->get_gameObject()->get_name();
                StringW humanName = names[i];
                getLogger().info("bone: %s %s", std::string(boneName).c_str(), std::string(humanName).c_str());
                humanBones[i] = UnityEngine::HumanBone(boneName, humanName, UnityEngine::HumanLimit(convertVector(bone.min), convertVector(bone.max), convertVector(bone.center), bone.axisLength, bone.useDefaultValues ? 0 : 1));
                getLogger().info("%p", humanBones[i].m_BoneName.convert());
                getLogger().info("%p", humanBones[i].m_HumanName.convert());
            }

            auto allObjs = root->GetComponentsInChildren<UnityEngine::Transform*>(true);
            ArrayW<UnityEngine::SkeletonBone> skeletonBones(allObjs.size());

            for (size_t i = 0; i < allObjs.size(); i++)
            {
                auto& obj = allObjs[i];
                skeletonBones[i] = UnityEngine::SkeletonBone();
                skeletonBones[i].name = obj->get_gameObject()->get_name();
                if(obj->get_parent() != nullptr)
                {
                    skeletonBones[i].parentName = obj->get_parent()->get_gameObject()->get_name();
                }
                else
                {
                    skeletonBones[i].parentName = "";
                }
                skeletonBones[i].position = obj->get_localPosition();
                skeletonBones[i].rotation = obj->get_localRotation();
                skeletonBones[i].scale = obj->get_localScale();
            }
            getLogger().info("x1");
            
            hd.human = humanBones;
            hd.skeleton = skeletonBones;
            getLogger().info("x2");
            hd.m_ArmStretch = humanoid.armStretch;
            hd.m_ArmTwist = humanoid.upperArmTwist;
            hd.m_FeetSpacing = humanoid.feetSpacing;
            hd.m_ForeArmTwist = humanoid.lowerArmTwist;
            hd.m_HasTranslationDoF = humanoid.hasTranslationDoF;
            hd.m_LegStretch = humanoid.legStretch;
            hd.m_LegTwist = humanoid.lowerLegTwist;
            hd.m_UpperLegTwist = humanoid.upperLegTwist;
            hd.m_RootMotionBoneName = "";

            getLogger().info("begin ptr %p", hd.human.begin());
            getLogger().info("convert ptr %p", hd.human.convert());
            try
            {
                getLogger().info("x3");
                static auto buildHumanoid = il2cpp_utils::resolve_icall<UnityEngine::Avatar*, UnityEngine::GameObject*, UnityEngine::HumanDescription*>("UnityEngine.AvatarBuilder::BuildHumanAvatarInternal_Injected");
                getLogger().info("x4");
                auto ava = buildHumanoid(root, &hd);
                getLogger().info("x5");
                return ava;
            }
            catch(const std::exception& e)
            {
                getLogger().error("%s", e.what());
            }
            return nullptr;
        }
    };
};