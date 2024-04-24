#include "customTypes/FinalIK/VR/Arm.hpp"
#include "customTypes/FinalIK/AxisTools.hpp"
#include "customTypes/FinalIK/Interp.hpp"
#include "customTypes/FinalIK/InterpolationMode.hpp"
#include "customTypes/FinalIK/V3Tools.hpp"
#include "customTypes/FinalIK/QuaTools.hpp"
#include "main.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

#include "customTypes/FinalIK/MathUtil.hpp"
#include <algorithm>

namespace VRMQavatars::FinalIK {

    void Arm::ApplyOffsets() {
        position += handPositionOffset;
    }

    float Arm::DamperValue(float value, float min, float max, float weight) {
        float num = max - min;
        if (weight < 1.0f)
        {
            float num2 = max - num * 0.5f;
            float num3 = value - num2;
            num3 *= 0.5f;
            value = num2 + num3;
        }
        value -= min;
        float num4 = Interp::Float(std::clamp(value / num, 0.0f, 1.0f), InterpolationMode::InOutSine);
        return std::lerp(min, max, num4);
    }

    Sombrero::FastVector3 Arm::GetBendNormal(Sombrero::FastVector3 dir) {
        if (bendGoal != nullptr)
        {
            bendDirection = Sombrero::FastVector3(bendGoal->position) - bones[1]->solverPosition;
        }
        auto vector = bones[0]->solverRotation * bones[0]->axis;
        auto down = Sombrero::FastVector3::down();
        auto vector2 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(chestRotation)) * dir.normalized + Sombrero::FastVector3::forward();
        auto vector3 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::FromToRotation(down, vector2)) * Sombrero::FastVector3::back();
        auto vector4 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(chestRotation)) * vector;
        vector2 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(chestRotation)) * dir;
        vector3 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::FromToRotation(vector4, vector2)) * vector3;
        vector3 = chestRotation * vector3;
        vector3 += vector;
        vector3 -= rotation * wristToPalmAxis;
        vector3 -= rotation * palmToThumbAxis * 0.5f;
        if (bendGoalWeight > 0.0f)
        {
            vector3 = Sombrero::FastVector3::Slerp(vector3, bendDirection, bendGoalWeight);
        }
        if (swivelOffset != 0.0f)
        {
            vector3 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(swivelOffset, -dir)) * vector3;
        }
        return Sombrero::FastVector3::Cross(vector3, dir);
    }

    void Arm::OnRead(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs, int rootIndex, int index) {
        Sombrero::FastVector3 vector = positions[index];
        Sombrero::FastQuaternion quaternion = rotations[index];
        Sombrero::FastVector3 vector2 = positions[index + 1];
        Sombrero::FastQuaternion quaternion2 = rotations[index + 1];
        Sombrero::FastVector3 vector3 = positions[index + 2];
        Sombrero::FastQuaternion quaternion3 = rotations[index + 2];
        Sombrero::FastVector3 vector4 = positions[index + 3];
        Sombrero::FastQuaternion quaternion4 = rotations[index + 3];
        if (!initiated)
        {
            IKPosition = vector4;
            IKRotation = quaternion4;
            rotation = IKRotation;
            hasShoulder = hasShoulders;
            bones = std::vector<VirtualBone*>(hasShoulder ? 4 : 3);
            if (hasShoulder)
            {
                bones[0] = new VirtualBone(vector, quaternion);
                bones[1] = new VirtualBone(vector2, quaternion2);
                bones[2] = new VirtualBone(vector3, quaternion3);
                bones[3] = new VirtualBone(vector4, quaternion4);
            }
            else
            {
                bones[0] = new VirtualBone(vector2, quaternion2);
                bones[1] = new VirtualBone(vector3, quaternion3);
                bones[2] = new VirtualBone(vector4, quaternion4);
            }
            Sombrero::FastVector3 vector5 = rotations[0] * Sombrero::FastVector3::forward();
            chestForwardAxis = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(rootRotation)) * vector5;
            chestUpAxis = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(rootRotation)) * (rotations[0] * Sombrero::FastVector3::up());
            Sombrero::FastVector3 vector6 = AxisTools::GetAxisVectorToDirection(quaternion2, vector5);
            if (Sombrero::FastVector3::Dot(quaternion2 * vector6, vector5) < 0.0f)
            {
                vector6 = -vector6;
            }
            upperArmBendAxis = Sombrero::FastVector3::Cross(Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(quaternion2)) * (vector3 - vector2), vector6);
        }
        if (hasShoulder)
        {
            bones[0]->Read(vector, quaternion);
            bones[1]->Read(vector2, quaternion2);
            bones[2]->Read(vector3, quaternion3);
            bones[3]->Read(vector4, quaternion4);
            return;
        }
        bones[0]->Read(vector2, quaternion2);
        bones[1]->Read(vector3, quaternion3);
        bones[2]->Read(vector4, quaternion4);
    }

    void Arm::PreSolve() {
        if (target != nullptr)
        {
            IKPosition = target->position;
            IKRotation = target->rotation;
        }
        position = V3Tools::Lerp(hand()->solverPosition, IKPosition, positionWeight);
        rotation = QuaTools::Lerp(hand()->solverRotation, IKRotation, rotationWeight);
        shoulder()->axis = shoulder()->axis.normalized;
        forearmRelToUpperArm = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(upperArm()->solverRotation)) * forearm()->solverRotation;
    }

    void Arm::ResetOffsets() {
        handPositionOffset = Sombrero::FastVector3::zero();
    }

    void Arm::Solve(bool isLeft) {
        VRMLogger.info("Arm::Solve {}", hasShoulder ? "has shoulder" : "no shoulder");
        chestRotation = Sombrero::FastQuaternion::LookRotation(rootRotation * chestForwardAxis, rootRotation * chestUpAxis);
        chestForward = chestRotation * Sombrero::FastVector3::forward();
        chestUp = chestRotation * Sombrero::FastVector3::up();
        Sombrero::FastVector3 vector = Sombrero::FastVector3::zero();
        if (hasShoulder && shoulderRotationWeight > 0.0f && LOD < 1)
        {
            ShoulderRotationMode shoulderRotationMode = this->shoulderRotationMode;
            if (shoulderRotationMode != ShoulderRotationMode::YawPitch)
            {
                if (shoulderRotationMode == ShoulderRotationMode::FromTo)
                {
                    Sombrero::FastQuaternion solverRotation = shoulder()->solverRotation;
                    Sombrero::FastQuaternion quaternion = Sombrero::FastQuaternion::FromToRotation((upperArm()->solverPosition - shoulder()->solverPosition).normalized + chestForward, position - shoulder()->solverPosition);
                    quaternion = Sombrero::FastQuaternion::Slerp(Sombrero::FastQuaternion::identity(), quaternion, 0.5f * shoulderRotationWeight * positionWeight);
                    VirtualBone::RotateBy(bones, quaternion);
                    Stretching();
                    VirtualBone::SolveTrigonometric(bones, 0, 2, 3, position, Sombrero::FastVector3::Cross(forearm()->solverPosition - shoulder()->solverPosition, hand()->solverPosition - shoulder()->solverPosition), 0.5f * shoulderRotationWeight * positionWeight);
                    vector = GetBendNormal(position - upperArm()->solverPosition);
                    VirtualBone::SolveTrigonometric(bones, 1, 2, 3, position, vector, positionWeight);
                    Sombrero::FastQuaternion quaternion2 = Sombrero::FastQuaternion::Inverse(Sombrero::FastQuaternion::LookRotation(chestUp, chestForward));
                    Sombrero::FastVector3 vector2 = quaternion2 * (solverRotation * shoulder()->axis);
                    Sombrero::FastVector3 vector3 = quaternion2 * (shoulder()->solverRotation * shoulder()->axis);
                    float num = std::atan2(vector2.x, vector2.z) * 57.29578f;
                    float num2 = std::atan2(vector3.x, vector3.z) * 57.29578f;
                    float num3 = DeltaAngle(num, num2);
                    if (isLeft)
                    {
                        num3 = -num3;
                    }
                    num3 = std::clamp(num3 * shoulderRotationWeight * shoulderTwistWeight * 2.0f * positionWeight, 0.0f, 180.0f);
                    shoulder()->solverRotation = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(num3, shoulder()->solverRotation * (isLeft ? shoulder()->axis : (-shoulder()->axis)))) * shoulder()->solverRotation;
                    upperArm()->solverRotation = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(num3, upperArm()->solverRotation * (isLeft ? upperArm()->axis : (-upperArm()->axis)))) * upperArm()->solverRotation;
                }
            }
            else
            {
                Sombrero::FastVector3 vector4 = (position - shoulder()->solverPosition).normalized;
                float num4 = (isLeft ? 45.0f : (-45.0f));
                Sombrero::FastQuaternion quaternion3 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis((isLeft ? (-90.0f) : 90.0f) + num4, chestUp)) * chestRotation;
                Sombrero::FastVector3 vector5 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(quaternion3)) * vector4;
                float num5 = std::atan2(vector5.x, vector5.z) * 57.29578f;
                float num6 = Sombrero::FastVector3::Dot(vector5, Sombrero::FastVector3::up());
                num6 = 1.0f - std::abs(num6);
                num5 *= num6;
                num5 -= num4;
                float num7 = (isLeft ? (-20.0f) : (-50.0f));
                float num8 = (isLeft ? 50.0f : 20.0f);
                num5 = DamperValue(num5, num7 - num4, num8 - num4, 0.7f);
                Sombrero::FastVector3 vector6 = shoulder()->solverRotation * shoulder()->axis;
                Sombrero::FastVector3 vector7 = quaternion3 * (Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(num5, Sombrero::FastVector3::up())) * Sombrero::FastVector3::forward());
                Sombrero::FastQuaternion quaternion4 = Sombrero::FastQuaternion::FromToRotation(vector6, vector7);
                quaternion3 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(isLeft ? (-90.0f) : 90.0f, chestUp)) * chestRotation;
                quaternion3 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(isLeft ? (-30.0f) : 30.0f, chestForward)) * quaternion3;
                vector4 = position - (shoulder()->solverPosition + chestRotation * (isLeft ? Sombrero::FastVector3::right() : Sombrero::FastVector3::left()) * mag);
                vector5 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(quaternion3)) * vector4;
                float num9 = std::atan2(vector5.y, vector5.z) * 57.29578f;
                num9 -= -30.0f;
                num9 = DamperValue(num9, -15.0f, 75.0f, 1.0f);
                Sombrero::FastQuaternion quaternion5 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(-num9, quaternion3 * Sombrero::FastVector3::right())) * quaternion4;
                if (shoulderRotationWeight * positionWeight < 1.0f)
                {
                    quaternion5 = Sombrero::FastQuaternion::Lerp(Sombrero::FastQuaternion::identity(), quaternion5, shoulderRotationWeight * positionWeight);
                }
                VirtualBone::RotateBy(bones, quaternion5);
                Stretching();
                vector = GetBendNormal(position - upperArm()->solverPosition);
                VirtualBone::SolveTrigonometric(bones, 1, 2, 3, position, vector, positionWeight);
                float num10 = std::clamp(num9 * positionWeight * shoulderRotationWeight * shoulderTwistWeight * 2.0f, 0.0f, 180.0f);
                shoulder()->solverRotation = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(num10, shoulder()->solverRotation * (isLeft ? shoulder()->axis : (-shoulder()->axis)))) * shoulder()->solverRotation;
                upperArm()->solverRotation = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(num10, upperArm()->solverRotation * (isLeft ? upperArm()->axis : (-upperArm()->axis)))) * upperArm()->solverRotation;
            }
        }
        else
        {
            if (LOD < 1)
            {
                Stretching();
            }
            vector = GetBendNormal(position - upperArm()->solverPosition);
            if (hasShoulder)
            {
                VirtualBone::SolveTrigonometric(bones, 1, 2, 3, position, vector, positionWeight);
            }
            else
            {
                VirtualBone::SolveTrigonometric(bones, 0, 1, 2, position, vector, positionWeight);
            }
        }
        if (LOD < 1)
        {
            Sombrero::FastVector3 vector8 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(Sombrero::FastQuaternion::LookRotation(upperArm()->solverRotation * upperArmBendAxis, forearm()->solverPosition - upperArm()->solverPosition))) * vector;
            float num11 = std::atan2(vector8.x, vector8.z) * 57.29578f;
            upperArm()->solverRotation = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(num11, forearm()->solverPosition - upperArm()->solverPosition)) * upperArm()->solverRotation;
            Sombrero::FastQuaternion quaternion6 = upperArm()->solverRotation * forearmRelToUpperArm;
            Sombrero::FastQuaternion quaternion7 = Sombrero::FastQuaternion::FromToRotation(quaternion6 * forearm()->axis, hand()->solverPosition - forearm()->solverPosition);
            RotateTo(forearm(), quaternion7 * quaternion6, positionWeight);
        }
        if (rotationWeight >= 1.0f)
        {
            hand()->solverRotation = rotation;
            return;
        }
        if (rotationWeight > 0.0f)
        {
            hand()->solverRotation = Sombrero::FastQuaternion::Lerp(hand()->solverRotation, rotation, rotationWeight);
        }
    }

    void Arm::Stretching() {
    }

    void Arm::Write(std::vector<Sombrero::FastVector3>& positions, std::vector<Sombrero::FastQuaternion>& rotations) {
        VRMLogger.info("Arm::Write {}", hasShoulder ? "has shoulder" : "no shoulder");
        if (hasShoulder)
        {
            positions[index] = shoulder()->solverPosition;
            rotations[index] = shoulder()->solverRotation;
        }
        positions[index + 1] = upperArm()->solverPosition;
        positions[index + 2] = forearm()->solverPosition;
        positions[index + 3] = hand()->solverPosition;
        rotations[index + 1] = upperArm()->solverRotation;
        rotations[index + 2] = forearm()->solverRotation;
        rotations[index + 3] = hand()->solverRotation;
    }

}