#pragma once

#include "Leg.hpp"
#include "Arm.hpp"
#include "UnityEngine/Transform.hpp"

#include "sombrero/shared/FastVector3.hpp"
#include "sombrero/shared/FastQuaternion.hpp"

namespace VRMQavatars::FinalIK {
    class Spine : public BodyPart {
    public:
        Spine();

        void AdjustChestByHands(Sombrero::FastQuaternion& chestTargetRotation, std::vector<Arm*> arms);
        void ApplyOffsets() override;
        void Bend(std::vector<VirtualBone*> bones, int firstIndex, int lastIndex, Sombrero::FastQuaternion targetRotation, float clampWeight, bool uniformWeight, float w);
        void Bend(std::vector<VirtualBone*> bones, int firstIndex, int lastIndex, Sombrero::FastQuaternion targetRotation, Sombrero::FastQuaternion rotationOffset, float clampWeight, bool uniformWeight, float w);
        void CalculateChestTargetRotation(VirtualBone* rootBone, std::vector<Arm*> arms);
        void FABRIKPass(Sombrero::FastVector3 animatedPelvisPos, Sombrero::FastVector3 rootUp, float weight);
        void InverseTranslateToHead(std::vector<Leg*> legs, bool limited, bool useCurrentLegMag, Sombrero::FastVector3 offset, float w);
        Sombrero::FastVector3 LimitPelvisPosition(std::vector<Leg*> legs, Sombrero::FastVector3 pelvisPosition, bool useCurrentLegMag, int it = 2);
        void OnRead(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs, int rootIndex, int index) override;
        void PreSolve() override;
        void ResetOffsets() override;
        void Solve(VirtualBone* rootBone, std::vector<Leg*> legs, std::vector<Arm*> arms);
        void SolvePelvis();
        void TranslatePelvis(std::vector<Leg*> legs, Sombrero::FastVector3 deltaPosition, Sombrero::FastQuaternion deltaRotation);
        void Write(std::vector<Sombrero::FastVector3>& positions, std::vector<Sombrero::FastQuaternion>& rotations) override;

        VirtualBone* pelvis() { return bones[pelvisIndex]; }
        VirtualBone* firstSpineBone() { return bones[spineIndex]; }
        VirtualBone* chest() { return hasChest ? bones[chestIndex] : bones[spineIndex]; }
        VirtualBone* neck() { return bones[neckIndex]; }
        VirtualBone* head() { return bones[headIndex]; }

        UnityEngine::Transform* headTarget;
        UnityEngine::Transform* pelvisTarget;
        float positionWeight = 1.0f;
        float rotationWeight = 1.0f;
        float pelvisPositionWeight;
        float pelvisRotationWeight;
        UnityEngine::Transform* chestGoal;
        float chestGoalWeight;
        float minHeadHeight = 0.8f;
        float bodyPosStiffness = 0.55f;
        float bodyRotStiffness = 0.1f;
        float neckStiffness = 0.2f;
        float rotateChestByHands = 1.0f;
        float chestClampWeight = 0.5f;
        float headClampWeight = 0.6f;
        float moveBodyBackWhenCrouching = 0.5f;
        float maintainPelvisPosition = 0.2f;
        float maxRootAngle = 25.0f;
        float rootHeadingOffset;
        Sombrero::FastVector3 IKPositionHead;
        Sombrero::FastQuaternion IKRotationHead = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 IKPositionPelvis;
        Sombrero::FastQuaternion IKRotationPelvis = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 goalPositionChest;
        Sombrero::FastVector3 pelvisPositionOffset;
        Sombrero::FastVector3 chestPositionOffset;
        Sombrero::FastVector3 headPositionOffset;
        Sombrero::FastQuaternion pelvisRotationOffset = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion chestRotationOffset = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion headRotationOffset = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 faceDirection;
        Sombrero::FastVector3 locomotionHeadPositionOffset;
        Sombrero::FastVector3 headPosition;
        Sombrero::FastQuaternion anchorRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion anchorRelativeToHead = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion headRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion pelvisRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion anchorRelativeToPelvis = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion pelvisRelativeRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion chestRelativeRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 headDeltaPosition;
        Sombrero::FastQuaternion pelvisDeltaRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion chestTargetRotation = Sombrero::FastQuaternion::identity();
        int pelvisIndex;
        int spineIndex = 1;
        int chestIndex = -1;
        int neckIndex = -1;
        int headIndex = -1;
        float length;
        bool hasChest;
        bool hasNeck;
        bool hasLegs;
        float headHeight;
        float sizeMlp;
        Sombrero::FastVector3 chestForward;
    };
}