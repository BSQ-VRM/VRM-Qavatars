#include "customTypes/FinalIK/VR/Footstep.hpp"
#include "customTypes/FinalIK/Interp.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Time.hpp"
#include <limits>
#include "main.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

namespace VRMQavatars::FinalIK {
    void Footstep::Reset(Sombrero::FastQuaternion rootRotation, Sombrero::FastVector3 footPosition, Sombrero::FastQuaternion footRotation) {
        position = footPosition;
        rotation = footRotation;
        stepFrom = position;
        stepTo = position;
        stepFromRot = rotation;
        stepToRot = rotation;
        stepToRootRot = rootRotation;
        stepProgress = 1.0f;
    }

    void Footstep::StepTo(Sombrero::FastVector3 p, Sombrero::FastQuaternion rootRotation, float stepThreshold) {
        if ((p - stepTo).Magnitude() < stepThreshold && Sombrero::FastQuaternion::Angle(rootRotation, stepToRootRot) < 25.0f)
        {
            return;
        }
        stepFrom = position;
        stepTo = p;
        stepFromRot = rotation;
        stepToRootRot = rootRotation;
        stepToRot = rootRotation * footRelativeToRoot;
        stepProgress = 0.0f;
    }

    void Footstep::Update(InterpolationMode interpolation) {
        float num = (isSupportLeg ? 1.0f : 0.0f);
        supportLegW = UnityEngine::Mathf::SmoothDamp(supportLegW, num, byref(supportLegWV), 0.2f, std::numeric_limits<float>::infinity(), UnityEngine::Time::get_deltaTime());
        if (!get_isStepping())
        {
            return;
        }
        stepProgress = UnityEngine::Mathf::MoveTowards(stepProgress, 1.0f, UnityEngine::Time::get_deltaTime() * stepSpeed);
        float num2 = Interp::Float(stepProgress, interpolation);
        VRMLogger.info("{}", num2);
        VRMLogger.info("{} {} {}", stepTo.x, stepTo.y, stepTo.z);
        position = Sombrero::FastVector3::Lerp(stepFrom, stepTo, num2);
        rotation = Sombrero::FastQuaternion::Lerp(stepFromRot, stepToRot, num2);
    }

    void Footstep::UpdateStanding(Sombrero::FastQuaternion rootRotation, float minAngle, float speed) {
        if (speed <= 0.0f || minAngle >= 180.0f)
        {
            return;
        }
        auto quaternion = rootRotation * footRelativeToRoot;
        float num = Sombrero::FastQuaternion::Angle(rotation, quaternion);
        if (num > minAngle)
        {
            rotation = Sombrero::FastQuaternion::RotateTowards(rotation, quaternion, std::min(UnityEngine::Time::get_deltaTime() * speed * (1.0f - supportLegW), num - minAngle));
        }
    }

    void Footstep::UpdateStepping(Sombrero::FastVector3 p, Sombrero::FastQuaternion rootRotation, float speed) {
        stepTo = Sombrero::FastVector3::Lerp(stepTo, p, UnityEngine::Time::get_deltaTime() * speed);
	    stepToRot = Sombrero::FastQuaternion::Lerp(stepToRot, rootRotation * footRelativeToRoot, UnityEngine::Time::get_deltaTime() * speed);
	    stepToRootRot = stepToRot * Sombrero::FastQuaternion::Inverse(footRelativeToRoot);
    }
}