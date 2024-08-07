#include "AssetLib/generators/avatarGenerator.hpp"

#include "UnityEngine/SkeletonBone.hpp"
#include "UnityEngine/HumanLimit.hpp"
#include "UnityEngine/HumanDescription.hpp"
#include "UnityEngine/HumanBone.hpp"
#include "UnityEngine/Transform.hpp"

namespace AssetLib::Generators
{
    //Mecanim names
    inline static const std::vector<std::string> names = {
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

    static UnityEngine::Vector3 convertVector(VRMC_VRM_0_0::Vector3 vec)
    {
        return UnityEngine::Vector3(vec.x, vec.y, vec.z);
    }

    static void Check(VRMC_VRM_1_0::HumanBone bone, VRMC_VRM_0_0::HumanoidBone::Bone type, std::map<VRMC_VRM_0_0::HumanoidBone::Bone, VRMC_VRM_1_0::HumanBone>& bones)
    {
        if(bone.node != 0) bones.emplace(type, bone);
    }

    template<>
    UnityEngine::Avatar* AvatarGenerator::Generate(const VRMC_VRM_0_0::Vrm& vrm, std::vector<AssetLib::Structure::Node*> bones, UnityEngine::GameObject* root)
    {
        auto humanoid = vrm.humanoid;
        auto hd = UnityEngine::HumanDescription();

        ArrayW<UnityEngine::HumanBone> humanBones(humanoid.humanBones.size());

        for (size_t i = 0; i < humanoid.humanBones.size(); i++)
        {
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
            if(bone.node+1 >= bones.size() || bones[bone.node+1]->gameObject == nullptr) continue;
            StringW boneName = bones[bone.node+1]->gameObject->get_name();
            humanBones[i] = UnityEngine::HumanBone(boneName, humanName, UnityEngine::HumanLimit(convertVector(bone.min), convertVector(bone.max), convertVector(bone.center), bone.axisLength, bone.useDefaultValues ? 0 : 1));
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
            static auto buildHumanoid = il2cpp_utils::resolve_icall<UnityEngine::Avatar*, UnityEngine::GameObject*, UnityEngine::HumanDescription*>("UnityEngine.AvatarBuilder::BuildHumanAvatarInternal_Injected");
            auto ava = buildHumanoid(root, &hd);
            return ava;
        }
        catch(const std::exception& e)
        {
            VRMLogger.error("{}", e.what());
        }
        return nullptr;
    }

    template<>
    UnityEngine::Avatar* AvatarGenerator::Generate(const VRMC_VRM_1_0::Vrm& vrm, std::vector<AssetLib::Structure::Node*> bones, UnityEngine::GameObject* root)
    {
        auto humanoid = vrm.humanoid;
        auto hd = UnityEngine::HumanDescription();

        std::map<VRMC_VRM_0_0::HumanoidBone::Bone, VRMC_VRM_1_0::HumanBone> bonesMap;

        Check(humanoid.humanBones.hips, VRMC_VRM_0_0::HumanoidBone::Bone::Hips, bonesMap);

        Check(humanoid.humanBones.leftUpperLeg, VRMC_VRM_0_0::HumanoidBone::Bone::LeftUpperLeg, bonesMap);
        Check(humanoid.humanBones.rightUpperLeg, VRMC_VRM_0_0::HumanoidBone::Bone::RightUpperLeg, bonesMap);

        Check(humanoid.humanBones.leftLowerLeg, VRMC_VRM_0_0::HumanoidBone::Bone::LeftLowerLeg, bonesMap);
        Check(humanoid.humanBones.rightLowerLeg, VRMC_VRM_0_0::HumanoidBone::Bone::RightLowerLeg, bonesMap);

        Check(humanoid.humanBones.leftFoot, VRMC_VRM_0_0::HumanoidBone::Bone::LeftFoot, bonesMap);
        Check(humanoid.humanBones.rightFoot, VRMC_VRM_0_0::HumanoidBone::Bone::RightFoot, bonesMap);

        Check(humanoid.humanBones.spine,  VRMC_VRM_0_0::HumanoidBone::Bone::Spine, bonesMap);
        Check(humanoid.humanBones.chest, VRMC_VRM_0_0::HumanoidBone::Bone::Chest, bonesMap);
        Check(humanoid.humanBones.neck,  VRMC_VRM_0_0::HumanoidBone::Bone::Neck, bonesMap);
        Check(humanoid.humanBones.head, VRMC_VRM_0_0::HumanoidBone::Bone::Head, bonesMap);

        Check(humanoid.humanBones.leftShoulder,  VRMC_VRM_0_0::HumanoidBone::Bone::LeftShoulder, bonesMap);
        Check(humanoid.humanBones.rightShoulder, VRMC_VRM_0_0::HumanoidBone::Bone::RightShoulder, bonesMap);

        Check(humanoid.humanBones.leftUpperArm,  VRMC_VRM_0_0::HumanoidBone::Bone::LeftUpperArm, bonesMap);
        Check(humanoid.humanBones.rightUpperArm, VRMC_VRM_0_0::HumanoidBone::Bone::RightUpperArm, bonesMap);

        Check(humanoid.humanBones.leftLowerArm,  VRMC_VRM_0_0::HumanoidBone::Bone::LeftLowerArm, bonesMap);
        Check(humanoid.humanBones.rightLowerArm, VRMC_VRM_0_0::HumanoidBone::Bone::RightLowerArm, bonesMap);

        Check(humanoid.humanBones.leftHand,  VRMC_VRM_0_0::HumanoidBone::Bone::LeftHand, bonesMap);
        Check(humanoid.humanBones.rightHand, VRMC_VRM_0_0::HumanoidBone::Bone::RightHand, bonesMap);

        Check(humanoid.humanBones.leftToes,  VRMC_VRM_0_0::HumanoidBone::Bone::LeftToes, bonesMap);
        Check(humanoid.humanBones.rightToes, VRMC_VRM_0_0::HumanoidBone::Bone::RightToes, bonesMap);

        Check(humanoid.humanBones.leftEye,  VRMC_VRM_0_0::HumanoidBone::Bone::LeftEye, bonesMap);
        Check(humanoid.humanBones.rightEye, VRMC_VRM_0_0::HumanoidBone::Bone::RightEye, bonesMap);

        Check(humanoid.humanBones.leftThumbProximal, VRMC_VRM_0_0::HumanoidBone::Bone::LeftThumbProximal, bonesMap);
        Check(humanoid.humanBones.leftThumbIntermediate, VRMC_VRM_0_0::HumanoidBone::Bone::LeftThumbIntermediate, bonesMap);
        Check(humanoid.humanBones.leftThumbDistal, VRMC_VRM_0_0::HumanoidBone::Bone::LeftThumbDistal, bonesMap);

        Check(humanoid.humanBones.leftThumbProximal, VRMC_VRM_0_0::HumanoidBone::Bone::LeftThumbProximal, bonesMap);
        Check(humanoid.humanBones.leftThumbIntermediate, VRMC_VRM_0_0::HumanoidBone::Bone::LeftThumbIntermediate, bonesMap);
        Check(humanoid.humanBones.leftThumbDistal, VRMC_VRM_0_0::HumanoidBone::Bone::LeftThumbDistal, bonesMap);

        Check(humanoid.humanBones.leftMiddleProximal, VRMC_VRM_0_0::HumanoidBone::Bone::LeftMiddleProximal, bonesMap);
        Check(humanoid.humanBones.leftMiddleIntermediate, VRMC_VRM_0_0::HumanoidBone::Bone::LeftMiddleIntermediate, bonesMap);
        Check(humanoid.humanBones.leftMiddleDistal, VRMC_VRM_0_0::HumanoidBone::Bone::LeftMiddleDistal, bonesMap);

        Check(humanoid.humanBones.leftRingProximal, VRMC_VRM_0_0::HumanoidBone::Bone::LeftRingProximal, bonesMap);
        Check(humanoid.humanBones.leftRingIntermediate, VRMC_VRM_0_0::HumanoidBone::Bone::LeftRingIntermediate, bonesMap);
        Check(humanoid.humanBones.leftRingDistal, VRMC_VRM_0_0::HumanoidBone::Bone::LeftRingDistal, bonesMap);

        Check(humanoid.humanBones.leftLittleProximal, VRMC_VRM_0_0::HumanoidBone::Bone::LeftLittleProximal, bonesMap);
        Check(humanoid.humanBones.leftLittleIntermediate, VRMC_VRM_0_0::HumanoidBone::Bone::LeftLittleIntermediate, bonesMap);
        Check(humanoid.humanBones.leftLittleDistal, VRMC_VRM_0_0::HumanoidBone::Bone::LeftLittleDistal, bonesMap);

        Check(humanoid.humanBones.rightThumbProximal, VRMC_VRM_0_0::HumanoidBone::Bone::RightThumbProximal, bonesMap);
        Check(humanoid.humanBones.rightThumbIntermediate, VRMC_VRM_0_0::HumanoidBone::Bone::RightThumbIntermediate, bonesMap);
        Check(humanoid.humanBones.rightThumbDistal, VRMC_VRM_0_0::HumanoidBone::Bone::RightThumbDistal, bonesMap);

        Check(humanoid.humanBones.rightThumbProximal, VRMC_VRM_0_0::HumanoidBone::Bone::RightThumbProximal, bonesMap);
        Check(humanoid.humanBones.rightThumbIntermediate, VRMC_VRM_0_0::HumanoidBone::Bone::RightThumbIntermediate, bonesMap);
        Check(humanoid.humanBones.rightThumbDistal, VRMC_VRM_0_0::HumanoidBone::Bone::RightThumbDistal, bonesMap);

        Check(humanoid.humanBones.rightMiddleProximal, VRMC_VRM_0_0::HumanoidBone::Bone::RightMiddleProximal, bonesMap);
        Check(humanoid.humanBones.rightMiddleIntermediate, VRMC_VRM_0_0::HumanoidBone::Bone::RightMiddleIntermediate, bonesMap);
        Check(humanoid.humanBones.rightMiddleDistal, VRMC_VRM_0_0::HumanoidBone::Bone::RightMiddleDistal, bonesMap);

        Check(humanoid.humanBones.rightRingProximal, VRMC_VRM_0_0::HumanoidBone::Bone::RightRingProximal, bonesMap);
        Check(humanoid.humanBones.rightRingIntermediate, VRMC_VRM_0_0::HumanoidBone::Bone::RightRingIntermediate, bonesMap);
        Check(humanoid.humanBones.rightRingDistal, VRMC_VRM_0_0::HumanoidBone::Bone::RightRingDistal, bonesMap);

        Check(humanoid.humanBones.rightLittleProximal, VRMC_VRM_0_0::HumanoidBone::Bone::RightLittleProximal, bonesMap);
        Check(humanoid.humanBones.rightLittleIntermediate, VRMC_VRM_0_0::HumanoidBone::Bone::RightLittleIntermediate, bonesMap);
        Check(humanoid.humanBones.rightLittleDistal, VRMC_VRM_0_0::HumanoidBone::Bone::RightLittleDistal, bonesMap);

        Check(humanoid.humanBones.upperChest, VRMC_VRM_0_0::HumanoidBone::Bone::UpperChest, bonesMap);

        ArrayW<UnityEngine::HumanBone> humanBones(bonesMap.size());

        std::map<VRMC_VRM_0_0::HumanoidBone::Bone, VRMC_VRM_1_0::HumanBone>::iterator it;

        int x = 0;
        for (it = bonesMap.begin(); it != bonesMap.end(); ++it)
        {
            auto bone = it->second;
            auto boneType = it->first;
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
            if(bone.node+1 >= bones.size() || bones[bone.node+1]->gameObject == nullptr) continue;
            StringW boneName = bones[bone.node+1]->gameObject->get_name();
            humanBones[x] = UnityEngine::HumanBone(boneName, humanName, UnityEngine::HumanLimit(Sombrero::FastVector3::zero(), Sombrero::FastVector3::zero(), Sombrero::FastVector3::zero(), 0, 1));
            x++;
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
        hd.m_ArmStretch = 0.0f;
        hd.m_ArmTwist = 0.0f;
        hd.m_FeetSpacing = 0.0f;
        hd.m_ForeArmTwist = 0.0f;
        hd.m_HasTranslationDoF = false;
        hd.m_LegStretch = 0.0f;
        hd.m_LegTwist = 0.0f;
        hd.m_UpperLegTwist = 0.0f;
        hd.m_RootMotionBoneName = "";
        try
        {
            static auto buildHumanoid = il2cpp_utils::resolve_icall<UnityEngine::Avatar*, UnityEngine::GameObject*, UnityEngine::HumanDescription*>("UnityEngine.AvatarBuilder::BuildHumanAvatarInternal_Injected");
            auto ava = buildHumanoid(root, &hd);
            return ava;
        }
        catch(const std::exception& e)
        {
            VRMLogger.error("{}", e.what());
        }
        return nullptr;
    }
}