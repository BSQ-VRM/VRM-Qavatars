#include "customTypes/FinalIK/Bone.hpp"
#include <iterator>

#include "customTypes/FinalIK/MathUtil.hpp"

namespace VRMQavatars::FinalIK {
    void Bone::SetToSolverPosition() {
        transform->position = solverPosition;
    }

    void Bone_SolverSwing(const std::vector<Bone*>& bones, int index, Sombrero::FastVector3 swingTarget, float weight) {
        if (weight <= 0.0f)
        {
            return;
        }
        Sombrero::FastQuaternion quaternion = Sombrero::FastQuaternion::FromToRotation(bones[index]->solverRotation * bones[index]->axis, swingTarget - bones[index]->solverPosition);
        if (weight >= 1.0f)
        {
            for (int i = index; i < bones.size(); i++)
            {
                bones[i]->solverRotation = quaternion * bones[i]->solverRotation;
            }
            return;
        }
        for (int j = index; j < bones.size(); j++)
        {
            Sombrero::FastQuaternion lerp = Sombrero::FastQuaternion::Lerp(Sombrero::FastQuaternion::identity(), quaternion, weight);
            bones[j]->solverRotation = lerp * bones[j]->solverRotation;
        }
    }

    void Bone::Swing(Sombrero::FastVector3 swingTarget, float weight) {
        if (weight <= 0.0f)
        {
            return;
        }
        Sombrero::FastQuaternion quaternion = Sombrero::FastQuaternion::FromToRotation(Sombrero::FastQuaternion(transform->rotation) * this->axis, swingTarget - transform->position);
        if (weight >= 1.0f)
        {
            transform->rotation = quaternion * transform->rotation;
            return;
        }
        Sombrero::FastQuaternion lerp = Sombrero::FastQuaternion::Lerp(Sombrero::FastQuaternion::identity(), quaternion, weight);
        transform->rotation = lerp * transform->rotation;
    }

    void Bone::Swing2D(Sombrero::FastVector3 swingTarget, float weight) {
        if (weight <= 0.0f)
        {
            return;
        }
        Sombrero::FastVector3 vector = Sombrero::FastQuaternion(transform->rotation) * axis;
        Sombrero::FastVector3 vector2 = swingTarget - transform->position;
        float num = std::atan2(vector.x, vector.y) * 57.29578f;
        float num2 = std::atan2(vector2.x, vector2.y) * 57.29578f;
        Sombrero::FastQuaternion axis = Sombrero::FastQuaternion::AngleAxis(DeltaAngle(num, num2) * weight, Sombrero::FastVector3::back());
        transform->rotation = axis * transform->rotation;
    }
}