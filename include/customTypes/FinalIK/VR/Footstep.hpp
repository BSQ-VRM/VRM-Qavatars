#pragma once

#include "customTypes/FinalIK/InterpolationMode.hpp"
#include "sombrero/shared/FastVector3.hpp"
#include "sombrero/shared/FastQuaternion.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"
#include "sombrero/shared/Vector3Utils.hpp"

namespace VRMQavatars::FinalIK {
    class Footstep {
    public:
        Footstep(Sombrero::FastQuaternion rootRotation, Sombrero::FastVector3 footPosition, Sombrero::FastQuaternion footRotation, Sombrero::FastVector3 characterSpaceOffset) {
            characterSpaceOffset = characterSpaceOffset;
            Reset(rootRotation, footPosition, footRotation);
            footRelativeToRoot = Sombrero::FastQuaternion(Sombrero::FastQuaternion::Inverse(rootRotation)) * rotation;
        }

        void Reset(Sombrero::FastQuaternion rootRotation, Sombrero::FastVector3 footPosition, Sombrero::FastQuaternion footRotation);
        void StepTo(Sombrero::FastVector3 p, Sombrero::FastQuaternion rootRotation, float stepThreshold);
        void Update(InterpolationMode interpolation);
        void UpdateStanding(Sombrero::FastQuaternion rootRotation, float minAngle, float speed);
        void UpdateStepping(Sombrero::FastVector3 p, Sombrero::FastQuaternion rootRotation, float speed);

        bool get_isStepping() const { return stepProgress < 1.0f; }
        
        float stepSpeed = 3.0f;
        Sombrero::FastVector3 characterSpaceOffset;
        Sombrero::FastVector3 position;
        Sombrero::FastQuaternion rotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion stepToRootRot = Sombrero::FastQuaternion::identity();
        bool isSupportLeg;
        float stepProgress;
        Sombrero::FastVector3 stepFrom;
        Sombrero::FastVector3 stepTo;
        Sombrero::FastQuaternion stepFromRot = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion stepToRot = Sombrero::FastQuaternion::identity();
        Sombrero::FastQuaternion footRelativeToRoot = Sombrero::FastQuaternion::identity();
        float supportLegW;
        float supportLegWV;
    };
}