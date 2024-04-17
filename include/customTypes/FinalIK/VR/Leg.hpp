#pragma once

#include "BodyPart.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/AnimationCurve.hpp"

namespace VRMQavatars::FinalIK {
    class Leg : public BodyPart {
    public:
        virtual ~Leg() = default;
        Leg();
        
        void ApplyOffsets() override;
        void ApplyPositionOffset(Sombrero::FastVector3 offset, float weight = 1.0f);
        void ApplyRotationOffset(Sombrero::FastQuaternion offset, float weight = 1.0f);
        void FixTwistRotations();
        void OnRead(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs, int rootIndex, int index) override;
        void PreSolve() override;
        void ResetOffsets() override;
        void Solve(bool isLeft);
        void Stretching();
        void Write(std::vector<Sombrero::FastVector3>& positions, std::vector<Sombrero::FastQuaternion>& rotations) override;

        VirtualBone* thigh() { return bones[0]; }
        VirtualBone* calf() { return bones[1]; }
        VirtualBone* foot() { return bones[2]; }
        VirtualBone* toes() { return bones[3]; }
        VirtualBone* lastBone() { return bones[bones.size() - 1]; }

        UnityEngine::Transform* target;
        UnityEngine::Transform* bendGoal;
        float positionWeight;
        float rotationWeight;
        float bendGoalWeight;
        float swivelOffset;
        float bendToTargetWeight = 0.5f;
        float legLengthMlp = 1.f;
        UnityEngine::AnimationCurve* stretchCurve;
        Sombrero::FastVector3 IKPosition;
        Sombrero::FastQuaternion IKRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 footPositionOffset;
        Sombrero::FastVector3 heelPositionOffset;
        Sombrero::FastQuaternion footRotationOffset = Sombrero::FastQuaternion::identity();
        float currentMag;
        bool useAnimatedBendNormal;
        Sombrero::FastVector3 position;
        Sombrero::FastQuaternion rotation;
        bool hasToes;
        Sombrero::FastVector3 thighRelativeToPelvis;
        Sombrero::FastVector3 footPosition;
        Sombrero::FastQuaternion footRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 bendNormal;
        Sombrero::FastQuaternion calfRelToThigh = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion thighRelToFoot = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 bendNormalRelToPelvis;
        Sombrero::FastVector3 bendNormalRelToTarget;
    };
}