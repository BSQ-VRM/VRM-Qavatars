#pragma once

#include "sombrero/shared/FastQuaternion.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

namespace VRMQavatars::FinalIK::QuaTools {
    Sombrero::FastQuaternion ClampRotation(Sombrero::FastQuaternion rotation, float clampWeight, int clampSmoothing);
    Sombrero::FastQuaternion FromToRotation(Sombrero::FastQuaternion from, Sombrero::FastQuaternion to);
    Sombrero::FastQuaternion Lerp(Sombrero::FastQuaternion fromRotation, Sombrero::FastQuaternion toRotation, float weight);
}