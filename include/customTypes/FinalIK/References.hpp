#pragma once

#include "main.hpp"

#include "UnityEngine/Animator.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/HumanBodyBones.hpp"

namespace VRMQavatars::FinalIK {
    class References {
    public:
        virtual ~References() = default;
        References() = default;

        std::vector<UnityEngine::Transform*> GetTransforms() {
            return {root, pelvis, spine, chest, neck, head, leftShoulder, leftUpperArm, leftForearm, leftHand, rightShoulder, rightUpperArm, rightForearm, rightHand, leftThigh, leftCalf, leftFoot, leftToes, rightThigh, rightCalf, rightFoot, rightToes};
        }

        bool isEmpty() {
            return !(root != nullptr) && !(pelvis != nullptr) && !(spine != nullptr) && !(chest != nullptr) && !(neck != nullptr) && !(head != nullptr) && !(leftShoulder != nullptr) && !(leftUpperArm != nullptr) && !(leftForearm != nullptr) && !(leftHand != nullptr) && !(rightShoulder != nullptr) && !(rightUpperArm != nullptr) && !(rightForearm != nullptr) && !(rightHand != nullptr) && !(leftThigh != nullptr) && !(leftCalf != nullptr) && !(leftFoot != nullptr) && !(leftToes != nullptr) && !(rightThigh != nullptr) && !(rightCalf != nullptr) && !(rightFoot != nullptr) && !(rightToes != nullptr);
        }

        bool isFilled() {
            return !(root == nullptr) && !(pelvis == nullptr) && !(spine == nullptr) && !(head == nullptr) && !(leftUpperArm == nullptr) && !(leftForearm == nullptr) && !(leftHand == nullptr) && !(rightUpperArm == nullptr) && !(rightForearm == nullptr) && !(rightHand == nullptr) && ((leftThigh == nullptr && leftCalf == nullptr && leftFoot == nullptr && rightThigh == nullptr && rightCalf == nullptr && rightFoot == nullptr) || (!(leftThigh == nullptr) && !(leftCalf == nullptr) && !(leftFoot == nullptr) && !(rightThigh == nullptr) && !(rightCalf == nullptr) && !(rightFoot == nullptr)));
        }

        UnityEngine::Transform* root = nullptr;
        UnityEngine::Transform* pelvis = nullptr;
        UnityEngine::Transform* spine = nullptr;
        UnityEngine::Transform* chest = nullptr;
        UnityEngine::Transform* neck = nullptr;
        UnityEngine::Transform* head = nullptr;
        UnityEngine::Transform* leftShoulder = nullptr;
        UnityEngine::Transform* leftUpperArm = nullptr;
        UnityEngine::Transform* leftForearm = nullptr;
        UnityEngine::Transform* leftHand = nullptr;
        UnityEngine::Transform* rightShoulder = nullptr;
        UnityEngine::Transform* rightUpperArm = nullptr;
        UnityEngine::Transform* rightForearm = nullptr;
        UnityEngine::Transform* rightHand = nullptr;
        UnityEngine::Transform* leftThigh = nullptr;
        UnityEngine::Transform* leftCalf = nullptr;
        UnityEngine::Transform* leftFoot = nullptr;
        UnityEngine::Transform* leftToes = nullptr;
        UnityEngine::Transform* rightThigh = nullptr;
        UnityEngine::Transform* rightCalf = nullptr;
        UnityEngine::Transform* rightFoot = nullptr;
        UnityEngine::Transform* rightToes = nullptr;
    };

    static std::optional<References*> AutoDetectReferences(UnityEngine::Transform* root) {
        auto references = new References();
        auto componentInChildren = root->GetComponentInChildren<UnityEngine::Animator*>();
        if (componentInChildren == nullptr || !componentInChildren->isHuman)
        {
            VRMLogger.error("VRIK needs a Humanoid Animator to auto-detect biped references. Please assign references manually.");
            return std::nullopt;
        }
        references->root = root;
        references->pelvis = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::Hips);
        references->spine = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::Spine);
        references->chest = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::Chest);
        references->neck = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::Neck);
        references->head = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::Head);
        references->leftShoulder = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::LeftShoulder);
        references->leftUpperArm = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::LeftUpperArm);
        references->leftForearm = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::LeftLowerArm);
        references->leftHand = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::LeftHand);
        references->rightShoulder = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::RightShoulder);
        references->rightUpperArm = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::RightUpperArm);
        references->rightForearm = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::RightLowerArm);
        references->rightHand = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::RightHand);
        references->leftThigh = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::LeftUpperLeg);
        references->leftCalf = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::LeftLowerLeg);
        references->leftFoot = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::LeftFoot);
        references->leftToes = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::LeftToes);
        references->rightThigh = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::RightUpperLeg);
        references->rightCalf = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::RightLowerLeg);
        references->rightFoot = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::RightFoot);
        references->rightToes = componentInChildren->GetBoneTransform(UnityEngine::HumanBodyBones::RightToes);
        return references;
    }
}