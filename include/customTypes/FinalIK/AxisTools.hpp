#pragma once 

#include "Axis.hpp"
#include "sombrero/shared/FastVector3.hpp"
#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

#include "UnityEngine/Transform.hpp"

namespace VRMQavatars::FinalIK::AxisTools {
    Sombrero::FastVector3 ToVector3(Axis axis);
    Axis GetAxisToDirection(UnityEngine::Transform* t, Sombrero::FastVector3 direction);
    Sombrero::FastVector3 GetAxisVectorToDirection(UnityEngine::Transform* t, Sombrero::FastVector3 direction);
    Sombrero::FastVector3 GetAxisVectorToDirection(Sombrero::FastQuaternion r, Sombrero::FastVector3 direction);
}