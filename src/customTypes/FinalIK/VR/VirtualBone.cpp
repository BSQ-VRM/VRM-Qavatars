#include "customTypes/FinalIK/VR/VirtualBone.hpp"
#include <cstddef>
#include <limits>

namespace VRMQavatars::FinalIK {
    void VirtualBone::Read(Sombrero::FastVector3 position, Sombrero::FastQuaternion rotation) {
        readPosition = position;
        readRotation = rotation;
        solverPosition = position;
        solverRotation = rotation;
    }

    void VirtualBone::SwingRotation(std::vector<VirtualBone*> bones, int index, Sombrero::FastVector3 swingTarget, float weight) {
        if (weight <= 0.0f)
        {
            return;
        }
        Sombrero::FastQuaternion quaternion = Sombrero::FastQuaternion::FromToRotation(bones[index]->solverRotation * bones[index]->axis, swingTarget - bones[index]->solverPosition);
        if (weight < 1.0f)
        {
            quaternion = Sombrero::FastQuaternion::Lerp(Sombrero::FastQuaternion::identity(), quaternion, weight);
        }
        for (int i = index; i < bones.size(); i++)
        {
            bones[i]->solverRotation = quaternion * bones[i]->solverRotation;
        }
    }

    float VirtualBone::PreSolve(std::vector<VirtualBone*>& bones) {
        float num = 0.0f;
        for (int i = 0; i < bones.size(); i++)
        {
            if (i < bones.size() - 1)
            {
                bones[i]->sqrMag = (bones[i + 1]->solverPosition - bones[i]->solverPosition).sqrMagnitude();
                bones[i]->length = std::sqrt(bones[i]->sqrMag);
                num += bones[i]->length;
                bones[i]->axis = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(bones[i]->solverRotation)) * (bones[i + 1]->solverPosition - bones[i]->solverPosition);
            }
            else
            {
                bones[i]->sqrMag = 0.0f;
                bones[i]->length = 0.0f;
            }
        }
        return num;
    }
    
    void VirtualBone::RotateAroundPoint(std::vector<VirtualBone*> bones, int index, Sombrero::FastVector3 point, Sombrero::FastQuaternion rotation) {
        for (int i = index; i < bones.size(); i++)
        {
            if (bones[i] != nullptr)
            {
                auto vector = bones[i]->solverPosition - point;
                bones[i]->solverPosition = point + rotation * vector;
                bones[i]->solverRotation = rotation * bones[i]->solverRotation;
            }
        }
    }
    
    void VirtualBone::RotateBy(std::vector<VirtualBone*> bones, Sombrero::FastQuaternion rotation) {
        for (int i = 0; i < bones.size(); i++)
        {
            if (bones[i] != nullptr)
            {
                if (i > 0)
                {
                    auto vector = bones[i]->solverPosition - bones[0]->solverPosition;
                    bones[i]->solverPosition = bones[0]->solverPosition + rotation * vector;
                }
                bones[i]->solverRotation = rotation * bones[i]->solverRotation;
            }
        }
    }
    
    void VirtualBone::SolveTrigonometric(std::vector<VirtualBone*> bones, int first, int second, int third, Sombrero::FastVector3 targetPosition, Sombrero::FastVector3 bendNormal, float weight) {
        if (weight <= 0.0f)
        {
            return;
        }
        targetPosition = Sombrero::FastVector3::Lerp(bones[third]->solverPosition, targetPosition, weight);
        auto vector = targetPosition - bones[first]->solverPosition;
        float sqrMagnitude = vector.sqrMagnitude();
        if (sqrMagnitude == 0.0f)
        {
            return;
        }
        float num = std::sqrt(sqrMagnitude);
        float sqrMagnitude2 = (bones[second]->solverPosition - bones[first]->solverPosition).sqrMagnitude();
        float sqrMagnitude3 = (bones[third]->solverPosition - bones[second]->solverPosition).sqrMagnitude();
        auto vector2 = Sombrero::FastVector3::Cross(vector, bendNormal);
        auto directionToBendPoint = VirtualBone::GetDirectionToBendPoint(vector, num, vector2, sqrMagnitude2, sqrMagnitude3);
        Sombrero::FastQuaternion quaternion = Sombrero::FastQuaternion::FromToRotation(bones[second]->solverPosition - bones[first]->solverPosition, directionToBendPoint);
        if (weight < 1.0f)
        {
            quaternion = Sombrero::FastQuaternion::Lerp(Sombrero::FastQuaternion::identity(), quaternion, weight);
        }
        VirtualBone::RotateAroundPoint(bones, first, bones[first]->solverPosition, quaternion);
        Sombrero::FastQuaternion quaternion2 = Sombrero::FastQuaternion::FromToRotation(bones[third]->solverPosition - bones[second]->solverPosition, targetPosition - bones[second]->solverPosition);
        if (weight < 1.0f)
        {
            quaternion2 = Sombrero::FastQuaternion::Lerp(Sombrero::FastQuaternion::identity(), quaternion2, weight);
        }
        VirtualBone::RotateAroundPoint(bones, second, bones[second]->solverPosition, quaternion2);
    }
    
    Sombrero::FastVector3 VirtualBone::GetDirectionToBendPoint(Sombrero::FastVector3 direction, float directionMag, Sombrero::FastVector3 bendDirection, float sqrMag1, float sqrMag2) {
        float num = (directionMag * directionMag + (sqrMag1 - sqrMag2)) / 2.0f / directionMag;
        float num2 = (float)std::sqrt((double)std::clamp(sqrMag1 - num * num, 0.0f, std::numeric_limits<float>::infinity()));
        if (direction == Sombrero::FastVector3::zero())
        {
            return Sombrero::FastVector3::zero();
        }
        return Sombrero::FastQuaternion(Sombrero::FastQuaternion::LookRotation(direction, bendDirection)) * Sombrero::FastVector3{0.0f, num2, num};
    }
    
    void VirtualBone::SolveFABRIK(std::vector<VirtualBone*> bones, Sombrero::FastVector3 startPosition, Sombrero::FastVector3 targetPosition, float weight, float minNormalizedTargetDistance, int iterations, float length, Sombrero::FastVector3 startOffset) {
        if (weight <= 0.0f)
        {
            return;
        }
        if (minNormalizedTargetDistance > 0.0f)
        {
            Sombrero::FastVector3 vector = targetPosition - startPosition;
            float magnitude = vector.magnitude;
            Sombrero::FastVector3 vector2 = startPosition + vector / magnitude * std::max(length * minNormalizedTargetDistance, magnitude);
            targetPosition = Sombrero::FastVector3::Lerp(targetPosition, vector2, weight);
        }
        for (int i = 0; i < iterations; i++)
        {
            bones[bones.size() - 1]->solverPosition = Sombrero::FastVector3::Lerp(bones[bones.size() - 1]->solverPosition, targetPosition, weight);
            for (size_t j = bones.size() - 2; j > -1; j--)
            {
                bones[j]->solverPosition = VirtualBone::SolveFABRIKJoint(bones[j]->solverPosition, bones[j + 1]->solverPosition, bones[j]->length);
            }
            if (i == 0)
            {
                for (int k = 0; k < bones.size(); k++)
                {
                    bones[k]->solverPosition += startOffset;
                }
            }
            bones[0]->solverPosition = startPosition;
            for (int l = 1; l < bones.size(); l++)
            {
                bones[l]->solverPosition = VirtualBone::SolveFABRIKJoint(bones[l]->solverPosition, bones[l - 1]->solverPosition, bones[l - 1]->length);
            }
        }
        for (int m = 0; m < bones.size() - 1; m++)
        {
            VirtualBone::SwingRotation(bones, m, bones[m + 1]->solverPosition, 1.0f);
        }
    }
    
    Sombrero::FastVector3 VirtualBone::SolveFABRIKJoint(Sombrero::FastVector3 pos1, Sombrero::FastVector3 pos2, float length) {
        return pos2 + (pos1 - pos2).normalized * length;
    }
}