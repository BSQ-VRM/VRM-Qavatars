#pragma once

#include "customTypes/FinalIK/Interp.hpp"
#include <cmath>
#include <limits>

#include "UnityEngine/Time.hpp"

namespace VRMQavatars::FinalIK::Interp {
    float Float(float t, InterpolationMode mode) {
        switch (mode)
        {
        case InterpolationMode::None:
            return None(t, 0.0f, 1.0f);
        case InterpolationMode::InOutQuintic:
            return InOutQuintic(t, 0.0f, 1.0f);
        case InterpolationMode::InOutSine:
            return VRMQavatars::FinalIK::Interp::InOutSine(t, 0.0f, 1.0f);
        }
        return 0.0f;
    }

    float InOutQuintic(float t, float b, float c) {
        float num = t * t;
        float num2 = num * t;
        return b + c * (6.0f * num2 * num + -15.0f * num * num + 10.0f * num2);
    }

    float InOutSine(float t, float b, float c) {
        c -= b;
	    return -c / 2.0f * (std::cos(3.1415927f * t / 1.0f) - 1.0f) + b;
    }

    float LerpValue(float value, float target, float increaseSpeed, float decreaseSpeed) {
        if (value == target)
        {
            return target;
        }
        if (value < target)
        {
            return std::clamp(value + UnityEngine::Time::get_deltaTime() * increaseSpeed, -std::numeric_limits<float>::infinity(), target);
        }
        return std::clamp(value - UnityEngine::Time::get_deltaTime() * decreaseSpeed, target, std::numeric_limits<float>::infinity());
    }

    float None(float t, float b, float c) {
        return b + c * t;
    }
}