#pragma once

#include "main.hpp"
#include "AssetLib/arrayUtils.hpp"

#include "vrmIncludes.hpp"

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
            auto humanoid = vrm.humanoid;
            auto hd = CustomHumanDescription();

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

            ArrayW<CustomHumanBone> humanBones(humanoid.humanBones.size());

            for (size_t i = 0; i < humanoid.humanBones.size(); i++)
            {
                auto& bone = humanoid.humanBones[i];
                StringW boneName = bones[bone.node]->gameObject->get_name();
                StringW humanName = names[i];
                getLogger().info("%s %s", static_cast<std::string>(boneName).c_str(), static_cast<std::string>(humanName).c_str());
                humanBones[i] = CustomHumanBone(boneName, humanName, UnityEngine::HumanLimit(convertVector(bone.min), convertVector(bone.max), convertVector(bone.center), bone.axisLength, bone.useDefaultValues ? 0 : 1));
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

            try
            {
                static auto buildHumanoid = il2cpp_utils::resolve_icall<UnityEngine::Avatar*, UnityEngine::GameObject*, CustomHumanDescription*>("UnityEngine.AvatarBuilder::BuildHumanAvatarInternal_Injected");
                auto ava = buildHumanoid(root, &hd);
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