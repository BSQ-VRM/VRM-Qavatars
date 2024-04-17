#include "customTypes/FinalIK/VR/Leg.hpp"
#include "customTypes/FinalIK/QuaTools.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"
#include "sombrero/shared/Vector3Utils.hpp"

namespace VRMQavatars::FinalIK {

    void Leg::ApplyOffsets() {
        ApplyPositionOffset(footPositionOffset, 1.0f);
        ApplyRotationOffset(footRotationOffset, 1.0f);
        Sombrero::FastQuaternion quaternion = Sombrero::FastQuaternion::FromToRotation(footPosition - position, footPosition + heelPositionOffset - position);
        footPosition = position + quaternion * (footPosition - position);
        footRotation = quaternion * footRotation;
        float num = 0.0f;
        if (bendGoal != nullptr && bendGoalWeight > 0.0f)
        {
            Sombrero::FastVector3 vector = Sombrero::FastVector3::Cross(Sombrero::FastVector3(bendGoal->position) - thigh()->solverPosition, position - thigh()->solverPosition);
            Sombrero::FastVector3 vector2 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(Sombrero::FastQuaternion::LookRotation(bendNormal, thigh()->solverPosition - foot()->solverPosition))) * vector;
            num = std::atan2(vector2.x, vector2.z) * 57.29578f * bendGoalWeight;
        }
        float num2 = swivelOffset + num;
        if (num2 != 0.0f)
        {
            bendNormal = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(num2, thigh()->solverPosition - lastBone()->solverPosition)) * bendNormal;
            thigh()->solverRotation = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(-num2, thigh()->solverRotation * thigh()->axis)) * thigh()->solverRotation;
        }
    }

    void Leg::ApplyPositionOffset(Sombrero::FastVector3 offset, float weight) {
        if (weight <= 0.0f)
        {
            return;
        }
        offset *= weight;
        footPosition += offset;
        position += offset;
    }

    void Leg::ApplyRotationOffset(Sombrero::FastQuaternion offset, float weight) {
        if (weight <= 0.0f)
        {
            return;
        }
        if (weight < 1.0f)
        {
            offset = Sombrero::FastQuaternion::Lerp(Sombrero::FastQuaternion::identity(), offset, weight);
        }
        footRotation = offset * footRotation;
        rotation = offset * rotation;
        bendNormal = offset * bendNormal;
        footPosition = position + offset * (footPosition - position);
    }

    void Leg::FixTwistRotations() {
        if (LOD < 1)
        {
            if (bendToTargetWeight > 0.0f)
            {
                Sombrero::FastQuaternion quaternion = rotation * thighRelToFoot;
                Sombrero::FastQuaternion quaternion2 = Sombrero::FastQuaternion::FromToRotation(quaternion * thigh()->axis, calf()->solverPosition - thigh()->solverPosition);
                if (bendToTargetWeight < 1.0f)
                {
                    thigh()->solverRotation = Sombrero::FastQuaternion::Slerp(thigh()->solverRotation, quaternion2 * quaternion, bendToTargetWeight);
                }
                else
                {
                    thigh()->solverRotation = quaternion2 * quaternion;
                }
            }
            Sombrero::FastQuaternion quaternion3 = thigh()->solverRotation * calfRelToThigh;
            Sombrero::FastQuaternion quaternion4 = Sombrero::FastQuaternion::FromToRotation(quaternion3 * calf()->axis, foot()->solverPosition - calf()->solverPosition);
            calf()->solverRotation = quaternion4 * quaternion3;
        }
    }

    void Leg::OnRead(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs, int rootIndex, int index) {
        auto vector = positions[index];
        auto quaternion = rotations[index];
        auto vector2 = positions[index + 1];
        auto quaternion2 = rotations[index + 1];
        auto vector3 = positions[index + 2];
        auto quaternion3 = rotations[index + 2];
        auto vector4 = positions[index + 3];
        auto quaternion4 = rotations[index + 3];
        if (!initiated)
        {
            hasToes = hasToes;
            bones = std::vector<VirtualBone*>(hasToes ? 4 : 3);
            if (hasToes)
            {
                bones[0] = new VirtualBone(vector, quaternion);
                bones[1] = new VirtualBone(vector2, quaternion2);
                bones[2] = new VirtualBone(vector3, quaternion3);
                bones[3] = new VirtualBone(vector4, quaternion4);
                IKPosition = vector4;
                IKRotation = quaternion4;
            }
            else
            {
                bones[0] = new VirtualBone(vector, quaternion);
                bones[1] = new VirtualBone(vector2, quaternion2);
                bones[2] = new VirtualBone(vector3, quaternion3);
                IKPosition = vector3;
                IKRotation = quaternion3;
            }
            bendNormal = Sombrero::FastVector3::Cross(vector2 - vector, vector3 - vector2);
            bendNormalRelToPelvis = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(rootRotation)) * bendNormal;
            bendNormalRelToTarget = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(IKRotation)) * bendNormal;
            rotation = IKRotation;
        }
        if (hasToes)
        {
            bones[0]->Read(vector, quaternion);
            bones[1]->Read(vector2, quaternion2);
            bones[2]->Read(vector3, quaternion3);
            bones[3]->Read(vector4, quaternion4);
            return;
        }
        bones[0]->Read(vector, quaternion);
        bones[1]->Read(vector2, quaternion2);
        bones[2]->Read(vector3, quaternion3);
    }

    void Leg::PreSolve() {
        if (target != nullptr)
        {
            IKPosition = target->position;
            IKRotation = target->rotation;
        }
        footPosition = foot()->solverPosition;
        footRotation = foot()->solverRotation;
        position = lastBone()->solverPosition;
        rotation = lastBone()->solverRotation;
        if (rotationWeight > 0.0f)
        {
            ApplyRotationOffset(QuaTools::FromToRotation(rotation, IKRotation), rotationWeight);
        }
        if (positionWeight > 0.0f)
        {
            ApplyPositionOffset(IKPosition - position, positionWeight);
        }
        thighRelativeToPelvis = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(rootRotation)) * (thigh()->solverPosition - rootPosition);
        calfRelToThigh = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(thigh()->solverRotation)) * calf()->solverRotation;
        thighRelToFoot = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(lastBone()->solverRotation)) * thigh()->solverRotation;
        if (useAnimatedBendNormal)
        {
            bendNormal = Sombrero::FastVector3::Cross(calf()->solverPosition - thigh()->solverPosition, foot()->solverPosition - calf()->solverPosition);
            return;
        }
        if (bendToTargetWeight <= 0.0f)
        {
            bendNormal = rootRotation * bendNormalRelToPelvis;
            return;
        }
        if (bendToTargetWeight >= 1.0f)
        {
            bendNormal = rotation * bendNormalRelToTarget;
            return;
        }
        bendNormal = Sombrero::FastVector3::Slerp(rootRotation * bendNormalRelToPelvis, rotation * bendNormalRelToTarget, bendToTargetWeight);
    }

    void Leg::ResetOffsets() {
        footPositionOffset = Sombrero::FastVector3::zero();
        footRotationOffset = Sombrero::FastQuaternion::identity();
        heelPositionOffset = Sombrero::FastVector3::zero();
    }

    void Leg::Solve(bool stretch) {
        if (stretch && LOD < 1)
        {
            Stretching();
        }
        VirtualBone::SolveTrigonometric(bones, 0, 1, 2, footPosition, bendNormal, 1.0f);
        RotateTo(foot(), footRotation, 1.0f);
        if (!hasToes)
        {
            FixTwistRotations();
            return;
        }
        Sombrero::FastVector3 vector = Sombrero::FastVector3::Cross(foot()->solverPosition - thigh()->solverPosition, toes()->solverPosition - foot()->solverPosition);
        VirtualBone::SolveTrigonometric(bones, 0, 2, 3, position, vector, 1.0f);
        FixTwistRotations();
        toes()->solverRotation = rotation;
    }

    void Leg::Stretching() {
        float num = thigh()->length + calf()->length;
        Sombrero::FastVector3 vector = Sombrero::FastVector3::zero();
        Sombrero::FastVector3 vector2 = Sombrero::FastVector3::zero();
        if (legLengthMlp != 1.0f)
        {
            num *= legLengthMlp;
            vector = (calf()->solverPosition - thigh()->solverPosition) * (legLengthMlp - 1.0f) * positionWeight;
            vector2 = (foot()->solverPosition - calf()->solverPosition) * (legLengthMlp - 1.0f) * positionWeight;
            calf()->solverPosition += vector;
            foot()->solverPosition += vector + vector2;
            if (hasToes)
            {
                toes()->solverPosition += vector + vector2;
            }
        }
        float num2 = thigh()->solverPosition.Distance(footPosition) / num;
        float num3 = stretchCurve->Evaluate(num2) * positionWeight;
        vector = (calf()->solverPosition - thigh()->solverPosition) * num3;
        vector2 = (foot()->solverPosition - calf()->solverPosition) * num3;
        calf()->solverPosition += vector;
        foot()->solverPosition += vector + vector2;
        if (hasToes)
        {
            toes()->solverPosition += vector + vector2;
        }
    }

    void Leg::Write(std::vector<Sombrero::FastVector3>& positions, std::vector<Sombrero::FastQuaternion>& rotations) {
        rotations[index] = thigh()->solverRotation;
        rotations[index + 1] = calf()->solverRotation;
        rotations[index + 2] = foot()->solverRotation;
        positions[index] = thigh()->solverPosition;
        positions[index + 1] = calf()->solverPosition;
        positions[index + 2] = foot()->solverPosition;
        if (hasToes)
        {
            rotations[index + 3] = toes()->solverRotation;
            positions[index + 3] = toes()->solverPosition;
        }
    }

}