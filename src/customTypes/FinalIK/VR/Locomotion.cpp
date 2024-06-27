#include "customTypes/FinalIK/VR/Locomotion.hpp"
#include "UnityEngine/zzzz__AnimationCurve_def.hpp"
#include "customTypes/FinalIK/V3Tools.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Physics.hpp"
#include "main.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"
#include <limits>

#include "customTypes/FinalIK/MathUtil.hpp"

namespace VRMQavatars::FinalIK {
    void Locomotion::Initiate(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasToes) {
        leftFootIndex = (hasToes ? 17 : 16);
        rightFootIndex = (hasToes ? 21 : 20);
        footsteps = 
        {
            new Footstep(rotations[0], positions[leftFootIndex], rotations[leftFootIndex], Sombrero::FastVector3::left() * footDistance),
            new Footstep(rotations[0], positions[rightFootIndex], rotations[rightFootIndex], Sombrero::FastVector3::right() * footDistance)
        };
    }

    void Locomotion::Reset(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations) {
        lastComPosition = Sombrero::FastVector3::Lerp(positions[1], positions[5], 0.25f) + rotations[0] * offset;
        comVelocity = Sombrero::FastVector3::zero();
        footsteps[0]->Reset(rotations[0], positions[leftFootIndex], rotations[leftFootIndex]);
        footsteps[1]->Reset(rotations[0], positions[rightFootIndex], rotations[rightFootIndex]);
    }

    void Locomotion::AddDeltaRotation(Sombrero::FastQuaternion delta, Sombrero::FastVector3 pivot) {
        auto vector = lastComPosition - pivot;
        lastComPosition = pivot + delta * vector;
        for (auto footstep : footsteps)
        {
            footstep->rotation = delta * footstep->rotation;
            footstep->stepFromRot = delta * footstep->stepFromRot;
            footstep->stepToRot = delta * footstep->stepToRot;
            footstep->stepToRootRot = delta * footstep->stepToRootRot;
            auto vector2 = footstep->position - pivot;
            footstep->position = pivot + delta * vector2;
            auto vector3 = footstep->stepFrom - pivot;
            footstep->stepFrom = pivot + delta * vector3;
            auto vector4 = footstep->stepTo - pivot;
            footstep->stepTo = pivot + delta * vector4;
        }
    }

    void Locomotion::AddDeltaPosition(Sombrero::FastVector3 delta) {
        lastComPosition += delta;
        for(auto footstep : footsteps)
        {
            footstep->position += delta;
            footstep->stepFrom += delta;
            footstep->stepTo += delta;
        }
    }

    void Locomotion::Solve(VirtualBone* rootBone, Spine* spine, Leg* leftLeg, Leg* rightLeg, Arm* leftArm, Arm* rightArm, int supportLegIndex, Sombrero::FastVector3& leftFootPosition, Sombrero::FastVector3& rightFootPosition, Sombrero::FastQuaternion& leftFootRotation, Sombrero::FastQuaternion& rightFootRotation, float& leftFootOffset, float& rightFootOffset, float& leftHeelOffset, float& rightHeelOffset) {
        if (weight <= 0.0f)
        {
            leftFootPosition = Sombrero::FastVector3::zero();
            rightFootPosition = Sombrero::FastVector3::zero();
            leftFootRotation = Sombrero::FastQuaternion::identity();
            rightFootRotation = Sombrero::FastQuaternion::identity();
            leftFootOffset = 0.0f;
            rightFootOffset = 0.0f;
            leftHeelOffset = 0.0f;
            rightHeelOffset = 0.0f;
            return;
        }
        auto vector = rootBone->solverRotation * Sombrero::FastVector3::up();
        auto vector2 = spine->pelvis()->solverPosition + spine->pelvis()->solverRotation * leftLeg->thighRelativeToPelvis;
        auto vector3 = spine->pelvis()->solverPosition + spine->pelvis()->solverRotation * rightLeg->thighRelativeToPelvis;
        footsteps[0]->characterSpaceOffset =  Sombrero::FastVector3::left() * footDistance;
        footsteps[1]->characterSpaceOffset =  Sombrero::FastVector3::right() * footDistance;
        auto faceDirection = spine->faceDirection;
        auto vector4 = V3Tools::ExtractVertical(faceDirection, vector, 1.0f);
        Sombrero::FastQuaternion quaternion = Sombrero::FastQuaternion::LookRotation(faceDirection - vector4, vector);
        if (spine->rootHeadingOffset != 0.0f)
        {
            quaternion = Sombrero::FastQuaternion(Sombrero::FastQuaternion::AngleAxis(spine->rootHeadingOffset, vector)) * quaternion;
        }
        float num = 1.0f;
        float num2 = 1.0f;
        float num3 = 0.2f;
        float num4 = num + num2 + 2.0f * num3;
        centerOfMass = Sombrero::FastVector3::zero();
        centerOfMass += spine->pelvis()->solverPosition * num;
        centerOfMass += spine->head()->solverPosition * num2;
        centerOfMass += leftArm->position * num3;
        centerOfMass += rightArm->position * num3;
        centerOfMass /= num4;
        centerOfMass += rootBone->solverRotation * offset;
        comVelocity = ((UnityEngine::Time::get_deltaTime() > 0.0f) ? ((centerOfMass - lastComPosition) / UnityEngine::Time::get_deltaTime()) : Sombrero::FastVector3::zero());
        lastComPosition = centerOfMass;
        comVelocity = ClampMagnitude(comVelocity, maxVelocity) * velocityFactor;
        auto vector5 = centerOfMass + comVelocity;
        auto vector6 = V3Tools::PointToPlane(spine->pelvis()->solverPosition, rootBone->solverPosition, vector);
        auto vector7 = V3Tools::PointToPlane(vector5, rootBone->solverPosition, vector);
        auto vector8 = Sombrero::FastVector3::Lerp(footsteps[0]->position, footsteps[1]->position, 0.5f);
        float num5 = Sombrero::FastVector3::Angle(vector5 - vector8, rootBone->solverRotation * Sombrero::FastVector3::up()) * comAngleMlp;
        for (int i = 0; i < footsteps.size(); i++)
        {
            footsteps[i]->isSupportLeg = supportLegIndex == i;
        }
        for (int j = 0; j < footsteps.size(); j++)
        {
            if (footsteps[j]->get_isStepping())
            {
                auto vector9 = vector7 + rootBone->solverRotation * footsteps[j]->characterSpaceOffset;
                if (!StepBlocked(footsteps[j]->stepFrom, vector9, rootBone->solverPosition))
                {
                    footsteps[j]->UpdateStepping(vector9, quaternion, 10.0f);
                }
            }
            else
            {
                footsteps[j]->UpdateStanding(quaternion, relaxLegTwistMinAngle, relaxLegTwistSpeed);
            }
        }
        if (CanStep())
        {
            int num6 = -1;
            float num7 = -std::numeric_limits<float>::infinity();
            for (int k = 0; k < footsteps.size(); k++)
            {
                if (!footsteps[k]->get_isStepping())
                {
                    auto vector10 = vector7 + rootBone->solverRotation * footsteps[k]->characterSpaceOffset;
                    float num8 = ((k == 0) ? leftLeg->mag : rightLeg->mag);
                    auto vector11 = ((k == 0) ? vector2 : vector3);
                    float num9 = footsteps[k]->position.Distance(vector11);
                    bool flag = false;
                    if (num9 >= num8 * maxLegStretch)
                    {
                        vector10 = vector6 + rootBone->solverRotation * footsteps[k]->characterSpaceOffset;
                        flag = true;
                    }
                    bool flag2 = false;
                    for (int l = 0; l < footsteps.size(); l++)
                    {
                        if (l != k && !flag)
                        {
                            if (footsteps[k]->position.Distance(footsteps[l]->position) >= 0.25f || (footsteps[k]->position - vector10).sqrMagnitude() >= (footsteps[l]->position - vector10).sqrMagnitude())
                            {
                                flag2 = GetLineSphereCollision(footsteps[k]->position, vector10, footsteps[l]->position, 0.25f);
                            }
                            if (flag2)
                            {
                                break;
                            }
                        }
                    }
                    float num10 = Sombrero::FastQuaternion::Angle(quaternion, footsteps[k]->stepToRootRot);
                    if (!flag2 || num10 > angleThreshold)
                    {
                        float num11 = footsteps[k]->position.Distance(vector10);
                        float num12 = std::lerp(stepThreshold, stepThreshold * 0.1f, num5 * 0.015f);
                        if (flag)
                        {
                            num12 *= 0.5f;
                        }
                        if (k == 0)
                        {
                            num12 *= 0.9f;
                        }
                        if (!StepBlocked(footsteps[k]->position, vector10, rootBone->solverPosition) && (num11 > num12 || num10 > angleThreshold))
                        {
                            float num13 = 0.0f;
                            num13 -= num11;
                            if (num13 > num7)
                            {
                                num6 = k;
                                num7 = num13;
                            }
                        }
                    }
                }
            }
            if (num6 != -1)
            {
                auto vector12 = vector7 + rootBone->solverRotation * footsteps[num6]->characterSpaceOffset;
                footsteps[num6]->stepSpeed = UnityEngine::Random::Range(stepSpeed, stepSpeed * 1.5f);
                footsteps[num6]->StepTo(vector12, quaternion, stepThreshold);
            }
        }
        footsteps[0]->Update(stepInterpolation);
        footsteps[1]->Update(stepInterpolation);
        leftFootPosition = footsteps[0]->position;
        rightFootPosition = footsteps[1]->position;
        leftFootPosition = V3Tools::PointToPlane(leftFootPosition, leftLeg->lastBone()->readPosition, vector);
        rightFootPosition = V3Tools::PointToPlane(rightFootPosition, rightLeg->lastBone()->readPosition, vector);

        auto evaluate = [](float progress)
        {
            float progNew = progress < 0.5f ? 4.0f * progress * progress * progress : 1.0f - std::pow(-2.0f * progress + 2.0f, 3.0f) / 2.0f;
            //Before half way point
            if(progNew < 0.5f) {
                return std::lerp(0.0f, 0.03f, progNew * 2.0f);
            } else { //Past half way point
                return std::lerp(0.03f, 0.0f, (progNew - 0.5f) * 2.0f);
            }
        };

        leftFootOffset = evaluate(footsteps[0]->stepProgress);
        rightFootOffset = evaluate(footsteps[1]->stepProgress);
        leftHeelOffset = evaluate(footsteps[0]->stepProgress);
        rightHeelOffset = evaluate(footsteps[1]->stepProgress);
        leftFootRotation = footsteps[0]->rotation;
        rightFootRotation = footsteps[1]->rotation;
    }

    bool Locomotion::StepBlocked(Sombrero::FastVector3 fromPosition, Sombrero::FastVector3 toPosition, Sombrero::FastVector3 rootPosition) {
        if (blockingLayers.value == -1 || !blockingEnabled)
        {
            return false;
        }
        auto vector = fromPosition;
        vector.y = rootPosition.y + raycastHeight + raycastRadius;
        auto vector2 = toPosition - vector;
        vector2.y = 0.0f;
        UnityEngine::RaycastHit raycastHit;
        if (raycastRadius <= 0.0f)
        {
            return UnityEngine::Physics::Raycast(vector, vector2, byref(raycastHit), vector2.magnitude, blockingLayers.value);
        }
        return UnityEngine::Physics::SphereCast(vector, raycastRadius, vector2, byref(raycastHit), vector2.magnitude, blockingLayers.value);
    }

    bool Locomotion::CanStep() {
        return std::ranges::all_of(footsteps, [](auto footstep) {
            return !footstep->get_isStepping() || footstep->stepProgress >= 0.8f;
        });
    }

    bool GetLineSphereCollision(Sombrero::FastVector3 lineStart, Sombrero::FastVector3 lineEnd, Sombrero::FastVector3 sphereCenter, float sphereRadius) {
        auto vector = lineEnd - lineStart;
        auto vector2 = sphereCenter - lineStart;
        float num = vector2.magnitude - sphereRadius;
        if (num > vector.magnitude)
        {
            return false;
        }
        auto vector3 = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(Sombrero::FastQuaternion::LookRotation(vector, vector2))) * vector2;
        if (vector3.z < 0.0f)
        {
            return num < 0.0f;
        }
        return vector3.y - sphereRadius < 0.0f;
    }
}