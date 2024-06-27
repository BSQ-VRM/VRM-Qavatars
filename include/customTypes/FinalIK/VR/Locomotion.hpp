#pragma once

#include "UnityEngine/AnimationCurve.hpp"
#include "UnityEngine/LayerMask.hpp"

#include "customTypes/FinalIK/InterpolationMode.hpp"
#include "Footstep.hpp"
#include "VirtualBone.hpp"
#include "Spine.hpp"
#include "Leg.hpp"
#include "Arm.hpp"

#include "sombrero/shared/FastVector3.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

namespace VRMQavatars::FinalIK {
    class Locomotion {
    public:
        Locomotion() = default;

        void Initiate(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasToes);
        void Reset(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations);
        void AddDeltaRotation(Sombrero::FastQuaternion delta, Sombrero::FastVector3 pivot);
        void AddDeltaPosition(Sombrero::FastVector3 delta);
        void Solve(VirtualBone* rootBone, Spine* spine, Leg* leftLeg, Leg* rightLeg, Arm* leftArm, Arm* rightArm, int supportLegIndex, Sombrero::FastVector3& leftFootPosition, Sombrero::FastVector3& rightFootPosition, Sombrero::FastQuaternion& leftFootRotation, Sombrero::FastQuaternion& rightFootRotation, float& leftFootOffset, float& rightFootOffset, float& leftHeelOffset, float& rightHeelOffset);
        Sombrero::FastVector3 leftFootstepPosition() { return footsteps[0]->position; }
        Sombrero::FastVector3 rightFootstepPosition() { return footsteps[1]->position; }
        Sombrero::FastQuaternion leftFootstepRotation() { return footsteps[0]->rotation; }
        Sombrero::FastQuaternion rightFootstepRotation() { return footsteps[1]->rotation; }
        bool StepBlocked(Sombrero::FastVector3 fromPosition, Sombrero::FastVector3 toPosition, Sombrero::FastVector3 rootPosition);
        bool CanStep();

        float weight = 1.0f;
        float footDistance = 0.3f;
        float stepThreshold = 0.4f;
        float angleThreshold = 60.0f;
        float comAngleMlp = 1.0f;
        float maxVelocity = 0.4f;
        float velocityFactor = 0.4f;
        float maxLegStretch = 1.0f;
        float rootSpeed = 20.0f;
        float stepSpeed = 3.0f;
        UnityEngine::AnimationCurve* stepHeight;
        UnityEngine::AnimationCurve* heelHeight;
        float relaxLegTwistMinAngle = 20.0f;
        float relaxLegTwistSpeed = 400.0f;
        InterpolationMode stepInterpolation = InterpolationMode::InOutSine;
        Sombrero::FastVector3 offset = Sombrero::FastVector3::zero();
        bool blockingEnabled = false;
        UnityEngine::LayerMask blockingLayers;
        float raycastRadius = 0.2f;
        float raycastHeight = 0.2f;
        Sombrero::FastVector3 centerOfMass = Sombrero::FastVector3::zero();
        std::vector<Footstep*> footsteps = std::vector<Footstep*>();
        Sombrero::FastVector3 lastComPosition = Sombrero::FastVector3::zero();
        Sombrero::FastVector3 comVelocity = Sombrero::FastVector3::zero();
        int leftFootIndex = 0;
        int rightFootIndex = 0;
    };

    bool GetLineSphereCollision(Sombrero::FastVector3 lineStart, Sombrero::FastVector3 lineEnd, Sombrero::FastVector3 sphereCenter, float sphereRadius);
}