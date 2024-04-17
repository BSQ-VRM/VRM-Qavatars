#include "customTypes/FinalIK/VR/Spine.hpp"
#include "customTypes/FinalIK/VR/VirtualBone.hpp"
#include "customTypes/FinalIK/V3Tools.hpp"
#include "customTypes/FinalIK/QuaTools.hpp"
#include <vector>

#include "customTypes/FinalIK/MathUtil.hpp"

namespace VRMQavatars::FinalIK {
    void Spine::AdjustChestByHands(Sombrero::FastQuaternion& chestTargetRotation, std::vector<Arm*> arms) {
        if (LOD > 0)
        {
            return;
        }
        Sombrero::FastQuaternion quaternion = Sombrero::FastQuaternion::Inverse(anchorRotation);
        Sombrero::FastVector3 vector = quaternion * (arms[0]->position - headPosition) / sizeMlp;
        Sombrero::FastVector3 vector2 = quaternion * (arms[1]->position - headPosition) / sizeMlp;
        Sombrero::FastVector3 forward = Sombrero::FastVector3::forward();
        forward.x += vector.x * std::abs(vector.x);
        forward.x += vector.z * std::abs(vector.z);
        forward.x += vector2.x * std::abs(vector2.x);
        forward.x -= vector2.z * std::abs(vector2.z);
        forward.x *= 5.0f * rotateChestByHands;
        Sombrero::FastQuaternion quaternion2 = Sombrero::FastQuaternion::AngleAxis(std::atan2(forward.x, forward.z) * 57.29578f, rootRotation * Sombrero::FastVector3::up());
        chestTargetRotation = quaternion2 * chestTargetRotation;
        Sombrero::FastVector3 up = Sombrero::FastVector3::up();
        up.x += vector.y;
        up.x -= vector2.y;
        up.x *= 0.5f * rotateChestByHands;
        quaternion2 = Sombrero::FastQuaternion::AngleAxis(std::atan2(up.x, up.y) * 57.29578f, rootRotation * Sombrero::FastVector3::back());
        chestTargetRotation = quaternion2 * chestTargetRotation;
    }
    
    void Spine::ApplyOffsets() {
        headPosition += headPositionOffset;
        Sombrero::FastVector3 vector = rootRotation * Sombrero::FastVector3::up();
        if (vector == Sombrero::FastVector3::up())
        {
            headPosition.y = std::max(rootPosition.y + minHeadHeight, headPosition.y);
        }
        else
        {
            Sombrero::FastVector3 vector2 = headPosition - rootPosition;
            Sombrero::FastVector3 vector3 = V3Tools::ExtractHorizontal(vector2, vector, 1.0f);
            Sombrero::FastVector3 vector4 = vector2 - vector3;
            if (Sombrero::FastVector3::Dot(vector4, vector) > 0.0f)
            {
                if (vector4.magnitude < minHeadHeight)
                {
                    vector4 = vector4.normalized * minHeadHeight;
                }
            }
            else
            {
                vector4 = -vector4.normalized * minHeadHeight;
            }
            headPosition = rootPosition + vector3 + vector4;
        }
        headRotation = headRotationOffset * headRotation;
        headDeltaPosition = headPosition - head()->solverPosition;
        pelvisDeltaRotation = QuaTools::FromToRotation(pelvis()->solverRotation, headRotation * pelvisRelativeRotation);
        if (pelvisRotationWeight <= 0.0f)
        {
            anchorRotation = headRotation * anchorRelativeToHead;
            return;
        }
        if (pelvisRotationWeight > 0.0f && pelvisRotationWeight < 1.0f)
        {
            anchorRotation = Sombrero::FastQuaternion::Lerp(headRotation * anchorRelativeToHead, pelvisRotation * anchorRelativeToPelvis, pelvisRotationWeight);
            return;
        }
        if (pelvisRotationWeight >= 1.0f)
        {
            anchorRotation = pelvisRotation * anchorRelativeToPelvis;
        }
    }
    
    void Spine::Bend(std::vector<VirtualBone*> bones, int firstIndex, int lastIndex, Sombrero::FastQuaternion targetRotation, float clampWeight, bool uniformWeight, float w) {
        if (w <= 0.0f)
        {
            return;
        }
        if (bones.size() == 0)
        {
            return;
        }
        int num = lastIndex + 1 - firstIndex;
        if (num < 1)
        {
            return;
        }
        Sombrero::FastQuaternion quaternion = QuaTools::FromToRotation(bones[lastIndex]->solverRotation, targetRotation);
        quaternion = QuaTools::ClampRotation(quaternion, clampWeight, 2);
        float num2 = (uniformWeight ? (1.0f / (float)num) : 0.0f);
        for (int i = firstIndex; i < lastIndex + 1; i++)
        {
            if (!uniformWeight)
            {
                num2 = std::clamp((float)((i - firstIndex + 1) / num), 0.0f, 1.0f);
            }
            VirtualBone::RotateAroundPoint(bones, i, bones[i]->solverPosition, Sombrero::FastQuaternion::Slerp(Sombrero::FastQuaternion::identity(), quaternion, num2 * w));
        }
    }
    
    void Spine::Bend(std::vector<VirtualBone*> bones, int firstIndex, int lastIndex, Sombrero::FastQuaternion targetRotation, Sombrero::FastQuaternion rotationOffset, float clampWeight, bool uniformWeight, float w) {
        if (w <= 0.0f)
        {
            return;
        }
        if (bones.size() == 0)
        {
            return;
        }
        int num = lastIndex + 1 - firstIndex;
        if (num < 1)
        {
            return;
        }
        Sombrero::FastQuaternion quaternion = QuaTools::FromToRotation(bones[lastIndex]->solverRotation, targetRotation);
        quaternion = QuaTools::ClampRotation(quaternion, clampWeight, 2);
        float num2 = (uniformWeight ? (1.0f / (float)num) : 0.0f);
        for (int i = firstIndex; i < lastIndex + 1; i++)
        {
            if (!uniformWeight)
            {
                if (num == 1)
                {
                    num2 = 1.0f;
                }
                else if (num == 2)
                {
                    num2 = ((i == 0) ? 0.2f : 0.8f);
                }
                else if (num == 3)
                {
                    if (i == 0)
                    {
                        num2 = 0.15f;
                    }
                    else if (i == 1)
                    {
                        num2 = 0.4f;
                    }
                    else
                    {
                        num2 = 0.45f;
                    }
                }
                else if (num > 3)
                {
                    num2 = 1.0f / (float)num;
                }
            }
            VirtualBone::RotateAroundPoint(bones, i, bones[i]->solverPosition, Sombrero::FastQuaternion::Slerp(Sombrero::FastQuaternion::Slerp(Sombrero::FastQuaternion::identity(), rotationOffset, num2), quaternion, num2 * w));
        }
    }
    
    void Spine::CalculateChestTargetRotation(VirtualBone* rootBone, std::vector<Arm*> arms) {
        chestTargetRotation = headRotation * chestRelativeRotation;
        AdjustChestByHands(chestTargetRotation, arms);
        faceDirection = Sombrero::FastVector3(Sombrero::FastVector3::Cross(anchorRotation * Sombrero::FastVector3::right(), rootBone->readRotation * Sombrero::FastVector3::up())) + anchorRotation * Sombrero::FastVector3::forward();
    }
    
    void Spine::FABRIKPass(Sombrero::FastVector3 animatedPelvisPos, Sombrero::FastVector3 rootUp, float weight) {
        Sombrero::FastVector3 vector = Sombrero::FastVector3::Lerp(pelvis()->solverPosition, animatedPelvisPos, maintainPelvisPosition) + pelvisPositionOffset;
        Sombrero::FastVector3 vector2 = headPosition - chestPositionOffset;
        Sombrero::FastVector3 zero = Sombrero::FastVector3::zero();
        float num = bones[0]->solverPosition.Distance(bones[bones.size() - 1]->solverPosition);
        VirtualBone::SolveFABRIK(bones, vector, vector2, weight, 1.0f, 1, num, zero);
    }
    
    void Spine::InverseTranslateToHead(std::vector<Leg*> legs, bool limited, bool useCurrentLegMag, Sombrero::FastVector3 offset, float w) {
        Sombrero::FastVector3 vector = (headPosition + offset - head()->solverPosition) * w;
        Sombrero::FastVector3 vector2 = pelvis()->solverPosition + vector;
        MovePosition(limited ? LimitPelvisPosition(legs, vector2, useCurrentLegMag, 2) : vector2);
    }
    
    Sombrero::FastVector3 Spine::LimitPelvisPosition(std::vector<Leg*> legs, Sombrero::FastVector3 pelvisPosition, bool useCurrentLegMag, int it) {
        if (!hasLegs)
        {
            return pelvisPosition;
        }
        if (useCurrentLegMag)
        {
            for (auto leg : legs)
            {
                leg->currentMag = leg->thigh()->solverPosition.Distance(leg->lastBone()->solverPosition);
            }
        }
        for (int j = 0; j < it; j++)
        {
            for (auto leg2 : legs)
            {
                Sombrero::FastVector3 vector = pelvisPosition - pelvis()->solverPosition;
                Sombrero::FastVector3 vector2 = leg2->thigh()->solverPosition + vector;
                Sombrero::FastVector3 vector3 = vector2 - leg2->position;
                float num = (useCurrentLegMag ? leg2->currentMag : leg2->mag);
                Sombrero::FastVector3 vector4 = leg2->position + ClampMagnitude(vector3, num);
                pelvisPosition += vector4 - vector2;
            }
        }
        return pelvisPosition;
    }
    
    void Spine::OnRead(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs, int rootIndex, int index) {
        Sombrero::FastVector3 vector = positions[index];
        Sombrero::FastQuaternion quaternion = rotations[index];
        Sombrero::FastVector3 vector2 = positions[index + 1];
        Sombrero::FastQuaternion quaternion2 = rotations[index + 1];
        Sombrero::FastVector3 vector3 = positions[index + 2];
        Sombrero::FastQuaternion quaternion3 = rotations[index + 2];
        Sombrero::FastVector3 vector4 = positions[index + 3];
        Sombrero::FastQuaternion quaternion4 = rotations[index + 3];
        Sombrero::FastVector3 vector5 = positions[index + 4];
        Sombrero::FastQuaternion quaternion5 = rotations[index + 4];
        hasLegs = hasLegs;
        if (!hasChest)
        {
            vector3 = vector2;
            quaternion3 = quaternion2;
        }
        if (!initiated)
        {
            hasChest = hasChest;
            hasNeck = hasNeck;
            headHeight = V3Tools::ExtractVertical(vector5 - positions[0], rotations[0] * Sombrero::FastVector3::up(), 1.0f).magnitude;
            int num = 3;
            if (hasChest)
            {
                num++;
            }
            if (hasNeck)
            {
                num++;
            }
            bones = std::vector<VirtualBone*>(num);
            chestIndex = (hasChest ? 2 : 1);
            neckIndex = 1;
            if (hasChest)
            {
                neckIndex++;
            }
            if (hasNeck)
            {
                neckIndex++;
            }
            headIndex = 2;
            if (hasChest)
            {
                headIndex++;
            }
            if (hasNeck)
            {
                headIndex++;
            }
            bones[0] = new VirtualBone(vector, quaternion);
            bones[1] = new VirtualBone(vector2, quaternion2);
            if (hasChest)
            {
                bones[chestIndex] = new VirtualBone(vector3, quaternion3);
            }
            if (hasNeck)
            {
                bones[neckIndex] = new VirtualBone(vector4, quaternion4);
            }
            bones[headIndex] = new VirtualBone(vector5, quaternion5);
            pelvisRotationOffset = Sombrero::FastQuaternion::identity();
            chestRotationOffset = Sombrero::FastQuaternion::identity();
            headRotationOffset = Sombrero::FastQuaternion::identity();
            anchorRelativeToHead = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(quaternion5)) * rotations[0];
            anchorRelativeToPelvis = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(quaternion)) * rotations[0];
            faceDirection = rotations[0] * Sombrero::FastVector3::forward();
            IKPositionHead = vector5;
            IKRotationHead = quaternion5;
            IKPositionPelvis = vector;
            IKRotationPelvis = quaternion;
            goalPositionChest = vector3 + rotations[0] * Sombrero::FastVector3::forward();
        }
        pelvisRelativeRotation = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(quaternion5)) * quaternion;
        chestRelativeRotation = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(quaternion5)) * quaternion3;
        chestForward = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(quaternion3)) * (rotations[0] * Sombrero::FastVector3::forward());
        bones[0]->Read(vector, quaternion);
        bones[1]->Read(vector2, quaternion2);
        if (hasChest)
        {
            bones[chestIndex]->Read(vector3, quaternion3);
        }
        if (hasNeck)
        {
            bones[neckIndex]->Read(vector4, quaternion4);
        }
        bones[headIndex]->Read(vector5, quaternion5);
        float num2 = vector.Distance(vector5);
        sizeMlp = num2 / 0.7f;
    }
    
    void Spine::PreSolve() {
        if (headTarget != nullptr)
        {
            IKPositionHead = headTarget->position;
            IKRotationHead = headTarget->rotation;
        }
        if (chestGoal != nullptr)
        {
            goalPositionChest = chestGoal->position;
        }
        if (pelvisTarget != nullptr)
        {
            IKPositionPelvis = pelvisTarget->position;
            IKRotationPelvis = pelvisTarget->rotation;
        }
        headPosition = V3Tools::Lerp(head()->solverPosition, IKPositionHead, positionWeight);
        headRotation = QuaTools::Lerp(head()->solverRotation, IKRotationHead, rotationWeight);
        pelvisRotation = QuaTools::Lerp(pelvis()->solverRotation, IKRotationPelvis, rotationWeight);
    }
    
    void Spine::ResetOffsets() {
        pelvisPositionOffset = Sombrero::FastVector3::zero();
        chestPositionOffset = Sombrero::FastVector3::zero();
        headPositionOffset = locomotionHeadPositionOffset;
        pelvisRotationOffset = Sombrero::FastQuaternion::identity();
        chestRotationOffset = Sombrero::FastQuaternion::identity();
        headRotationOffset = Sombrero::FastQuaternion::identity();
    }
    
    void Spine::Solve(VirtualBone* rootBone, std::vector<Leg*> legs, std::vector<Arm*> arms) {
        CalculateChestTargetRotation(rootBone, arms);
        if (maxRootAngle < 180.0f)
        {
            Sombrero::FastVector3 vector = faceDirection;
            if (rootHeadingOffset != 0.0f)
            {
                vector = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(rootHeadingOffset, Sombrero::FastVector3::up())) * vector;
            }
            Sombrero::FastVector3 vector2 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(rootBone->solverRotation)) * vector;
            float num = std::atan2(vector2.x, vector2.z) * 57.29578f;
            float num2 = 0.0f;
            float num3 = maxRootAngle;
            if (num > num3)
            {
                num2 = num - num3;
            }
            if (num < -num3)
            {
                num2 = num + num3;
            }
            rootBone->solverRotation = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(num2, rootBone->readRotation * Sombrero::FastVector3::up())) * rootBone->solverRotation;
        }
        Sombrero::FastVector3 solverPosition = pelvis()->solverPosition;
        Sombrero::FastVector3 vector3 = rootBone->solverRotation * Sombrero::FastVector3::up();
        TranslatePelvis(legs, headDeltaPosition, pelvisDeltaRotation);
        FABRIKPass(solverPosition, vector3, positionWeight);
        Bend(bones, pelvisIndex, chestIndex, chestTargetRotation, chestRotationOffset, chestClampWeight, false, neckStiffness * rotationWeight);
        if (LOD < 1 && chestGoalWeight > 0.0f)
        {
            Sombrero::FastQuaternion quaternion = Sombrero::FastQuaternion(Sombrero::FastQuaternion::FromToRotation(bones[chestIndex]->solverRotation * chestForward, goalPositionChest - bones[chestIndex]->solverPosition)) * bones[chestIndex]->solverRotation;
            Bend(bones, pelvisIndex, chestIndex, quaternion, chestRotationOffset, chestClampWeight, false, chestGoalWeight * rotationWeight);
        }
        InverseTranslateToHead(legs, false, false, Sombrero::FastVector3::zero(), positionWeight);
        if (LOD < 1)
        {
            FABRIKPass(solverPosition, vector3, positionWeight);
        }
        Bend(bones, neckIndex, headIndex, headRotation, headClampWeight, true, rotationWeight);
        SolvePelvis();
    }
    
    void Spine::SolvePelvis() {
        if (pelvisPositionWeight > 0.0f)
        {
            Sombrero::FastQuaternion solverRotation = head()->solverRotation;
            Sombrero::FastVector3 vector = (IKPositionPelvis + pelvisPositionOffset - pelvis()->solverPosition) * pelvisPositionWeight;
            auto bones = this->bones;
            for (int i = 0; i < bones.size(); i++)
            {
                bones[i]->solverPosition += vector;
            }
            Sombrero::FastVector3 vector2 = anchorRotation * Sombrero::FastVector3::right();
            if (hasChest && hasNeck)
            {
                VirtualBone::SolveTrigonometric(bones, pelvisIndex, spineIndex, headIndex, headPosition, vector2, pelvisPositionWeight * 0.6f);
                VirtualBone::SolveTrigonometric(bones, pelvisIndex, chestIndex, headIndex, headPosition, vector2, pelvisPositionWeight * 0.6f);
                VirtualBone::SolveTrigonometric(bones, pelvisIndex, neckIndex, headIndex, headPosition, vector2, pelvisPositionWeight * 1.0f);
            }
            else if (hasChest && !hasNeck)
            {
                VirtualBone::SolveTrigonometric(bones, pelvisIndex, spineIndex, headIndex, headPosition, vector2, pelvisPositionWeight * 0.75f);
                VirtualBone::SolveTrigonometric(bones, pelvisIndex, chestIndex, headIndex, headPosition, vector2, pelvisPositionWeight * 1.0f);
            }
            else if (!hasChest && hasNeck)
            {
                VirtualBone::SolveTrigonometric(bones, pelvisIndex, spineIndex, headIndex, headPosition, vector2, pelvisPositionWeight * 0.75f);
                VirtualBone::SolveTrigonometric(bones, pelvisIndex, neckIndex, headIndex, headPosition, vector2, pelvisPositionWeight * 1.0f);
            }
            else if (!hasNeck && !hasChest)
            {
                VirtualBone::SolveTrigonometric(bones, pelvisIndex, spineIndex, headIndex, headPosition, vector2, pelvisPositionWeight);
            }
            head()->solverRotation = solverRotation;
        }
    }
    
    void Spine::TranslatePelvis(std::vector<Leg*> legs, Sombrero::FastVector3 deltaPosition, Sombrero::FastQuaternion deltaRotation) {
        Sombrero::FastVector3 solverPosition = head()->solverPosition;
        deltaRotation = QuaTools::ClampRotation(deltaRotation, chestClampWeight, 2);
        Sombrero::FastQuaternion quaternion = Sombrero::FastQuaternion::Slerp(Sombrero::FastQuaternion::identity(), deltaRotation, bodyRotStiffness * rotationWeight);
        quaternion = Sombrero::FastQuaternion::Slerp(quaternion, QuaTools::FromToRotation(pelvis()->solverRotation, IKRotationPelvis), pelvisRotationWeight);
        VirtualBone::RotateAroundPoint(bones, 0, pelvis()->solverPosition, pelvisRotationOffset * quaternion);
        deltaPosition -= head()->solverPosition - solverPosition;
        Sombrero::FastVector3 vector = rootRotation * Sombrero::FastVector3::forward();
        float num = V3Tools::ExtractVertical(deltaPosition, rootRotation * Sombrero::FastVector3::up(), 1.0f).magnitude * -moveBodyBackWhenCrouching * headHeight;
        deltaPosition += vector * num;
        MovePosition(LimitPelvisPosition(legs, pelvis()->solverPosition + deltaPosition * bodyPosStiffness * positionWeight, false, 2));
    }
    
    void Spine::Write(std::vector<Sombrero::FastVector3>& positions, std::vector<Sombrero::FastQuaternion>& rotations) {
        positions[index] = bones[0]->solverPosition;
        rotations[index] = bones[0]->solverRotation;
        rotations[index + 1] = bones[1]->solverRotation;
        if (hasChest)
        {
            rotations[index + 2] = bones[chestIndex]->solverRotation;
        }
        if (hasNeck)
        {
            rotations[index + 3] = bones[neckIndex]->solverRotation;
        }
        rotations[index + 4] = bones[headIndex]->solverRotation;
    }
    
}