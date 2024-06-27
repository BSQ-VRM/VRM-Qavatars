#pragma once

#include "InterpolationMode.hpp"

namespace VRMQavatars::FinalIK::Interp {
    float Float(float t, InterpolationMode mode);
    float InOutQuintic(float t, float b, float c);
    float InOutSine(float t, float b, float c);
    float LerpValue(float value, float target, float increaseSpeed, float decreaseSpeed);
    float None(float t, float b, float c);
}