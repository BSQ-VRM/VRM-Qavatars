#pragma once

#include "main.hpp"
#include "AssetLib/arrayUtils.hpp"

#include "vrm/vrmIncludes.hpp"

#include "UnityEngine/Avatar.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SkeletonBone.hpp"

#include "AssetLib/skinTypes/newHumanBone.hpp"
#include "AssetLib/skinTypes/newHumanDescription.hpp"
#include "AssetLib/structure/node.hpp"

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
        static UnityEngine::Avatar* CreateAvatar(VRMC_VRM_0_0::Vrm vrm, std::vector<AssetLib::Structure::Node*> bones, UnityEngine::GameObject* root)
        {
            getLogger().info("%lu", bones.size());
            getLogger().info("mappings x1");
            auto humanoid = vrm.humanoid;
            auto hd = CustomHumanDescription();
            getLogger().info("mappings x2");
            //Mecanim names
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
            getLogger().info("mappings x3");
            ArrayW<CustomHumanBone> humanBones(humanoid.humanBones.size());

            for (size_t i = 0; i < humanoid.humanBones.size(); i++)
            {
                getLogger().info("mappings x4");
                auto& bone = humanoid.humanBones[i];
                auto boneType = bone.bone;
                StringW humanName = "";

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::Hips) humanName = names[0];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftUpperLeg) humanName = names[1];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightUpperLeg) humanName = names[2];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftLowerLeg) humanName = names[3];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightLowerLeg) humanName = names[4];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftFoot) humanName = names[5];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightFoot) humanName = names[6];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::Spine) humanName = names[7];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::Chest) humanName = names[8];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::Neck) humanName = names[9];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::Head) humanName = names[10];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftShoulder) humanName = names[11];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightShoulder) humanName = names[12];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftUpperArm) humanName = names[13];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightUpperArm) humanName = names[14];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftLowerArm) humanName = names[15];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightLowerArm) humanName = names[16];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftHand) humanName = names[17];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightHand) humanName = names[18];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftToes) humanName = names[19];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightToes) humanName = names[20];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftEye) humanName = names[21];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightEye) humanName = names[22];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftThumbProximal) humanName = names[23];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftThumbIntermediate) humanName = names[24];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftThumbDistal) humanName = names[25];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftIndexProximal) humanName = names[26];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftIndexIntermediate) humanName = names[27];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftIndexDistal) humanName = names[28];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftMiddleProximal) humanName = names[29];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftMiddleIntermediate) humanName = names[30];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftMiddleDistal) humanName = names[31];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftRingProximal) humanName = names[32];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftRingIntermediate) humanName = names[33];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftRingDistal) humanName = names[34];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftLittleProximal) humanName = names[35];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftLittleIntermediate) humanName = names[36];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::LeftLittleDistal) humanName = names[37];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightThumbProximal) humanName = names[38];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightThumbIntermediate) humanName = names[39];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightThumbDistal) humanName = names[40];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightIndexProximal) humanName = names[41];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightIndexIntermediate) humanName = names[42];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightIndexDistal) humanName = names[43];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightMiddleProximal) humanName = names[44];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightMiddleIntermediate) humanName = names[45];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightMiddleDistal) humanName = names[46];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightRingProximal) humanName = names[47];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightRingIntermediate) humanName = names[48];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightRingDistal) humanName = names[49];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightLittleProximal) humanName = names[50];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightLittleIntermediate) humanName = names[51];
                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::RightLittleDistal) humanName = names[52];

                if(boneType == VRMC_VRM_0_0::HumanoidBone::Bone::UpperChest) humanName = names[53];
                getLogger().info("mappings x5");
                if(bone.node >= bones.size() || bones[bone.node]->gameObject == nullptr) continue;
                getLogger().info("mappings x5.5");
                StringW boneName = bones[bone.node]->gameObject->get_name();
                getLogger().info("x");
                getLogger().info("mappings x6");
                getLogger().info("bones %s ___ %s", static_cast<std::string>(boneName).c_str(), static_cast<std::string>(humanName).c_str());
                humanBones[i] = CustomHumanBone(boneName, humanName, UnityEngine::HumanLimit(convertVector(bone.min), convertVector(bone.max), convertVector(bone.center), bone.axisLength, bone.useDefaultValues ? 0 : 1));
                getLogger().info("mappings x7");
            }
            getLogger().info("mappings x8");
            auto allObjs = root->GetComponentsInChildren<UnityEngine::Transform*>(true);
            ArrayW<UnityEngine::SkeletonBone> skeletonBones(allObjs.size());
            getLogger().info("mappings x9");

            for (size_t i = 0; i < allObjs.size(); i++)
            {
                getLogger().info("mappings x10");
                auto& obj = allObjs[i];
                skeletonBones[i] = UnityEngine::SkeletonBone();
                skeletonBones[i].name = obj->get_gameObject()->get_name();
                getLogger().info("mappings x11");
                if(obj->get_parent() != nullptr)
                {
                    skeletonBones[i].parentName = obj->get_parent()->get_gameObject()->get_name();
                }
                else
                {
                    skeletonBones[i].parentName = "";
                }
                getLogger().info("mappings x12");
                skeletonBones[i].position = obj->get_localPosition();
                skeletonBones[i].rotation = obj->get_localRotation();
                skeletonBones[i].scale = obj->get_localScale();
                getLogger().info("mappings x3");
            }
            getLogger().info("mappings x14");
            hd.human = humanBones;
            hd.skeleton = skeletonBones;
            hd.m_ArmStretch = humanoid.armStretch;
            hd.m_ArmTwist = humanoid.upperArmTwist;
            hd.m_FeetSpacing = humanoid.feetSpacing;
            hd.m_ForeArmTwist = humanoid.lowerArmTwist;
            hd.m_HasTranslationDoF = humanoid.hasTranslationDoF;
            hd.m_LegStretch = humanoid.legStretch;
            hd.m_LegTwist = humanoid.lowerLegTwist;
            hd.m_UpperLegTwist = humanoid.upperLegTwist;
            hd.m_RootMotionBoneName = "";
            getLogger().info("mappings x15");
            try
            {
                getLogger().info("mappings x16");
                static auto buildHumanoid = il2cpp_utils::resolve_icall<UnityEngine::Avatar*, UnityEngine::GameObject*, CustomHumanDescription*>("UnityEngine.AvatarBuilder::BuildHumanAvatarInternal_Injected");
                auto ava = buildHumanoid(root, &hd);
                return ava;
            }
            catch(const std::exception& e)
            {
                getLogger().error("%s", e.what());
            }
            getLogger().info("mappings x14");
            return nullptr;
        }
    };
};