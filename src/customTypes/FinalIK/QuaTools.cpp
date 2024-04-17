#include "sombrero/shared/FastQuaternion.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

namespace VRMQavatars::FinalIK::QuaTools {
    Sombrero::FastQuaternion ClampRotation(Sombrero::FastQuaternion rotation, float clampWeight, int clampSmoothing) {
        if (clampWeight >= 1.0f)
        {
            return Sombrero::FastQuaternion::identity();
        }
        if (clampWeight <= 0.0f)
        {
            return rotation;
        }
        float num = Sombrero::FastQuaternion::Angle(Sombrero::FastQuaternion::identity(), rotation);
        float num2 = 1.0f - num / 180.0f;
        float num3 = std::clamp(1.0f - (clampWeight - num2) / (1.0f - num2), 0.0f, 1.0f);
        float num4 = std::clamp(num2 / clampWeight, 0.0f, 1.0f);
        for (int i = 0; i < clampSmoothing; i++)
        {
            num4 = std::sin(num4 * 3.1415927f * 0.5f);
        }
        return Sombrero::FastQuaternion::Slerp(Sombrero::FastQuaternion::identity(), rotation, num4 * num3);
    }

    Sombrero::FastQuaternion FromToRotation(Sombrero::FastQuaternion from, Sombrero::FastQuaternion to) {
        if (to == from)
        {
            return Sombrero::FastQuaternion::identity();
        }
        return to * Sombrero::FastQuaternion::Inverse(from);
    }

    Sombrero::FastQuaternion Lerp(Sombrero::FastQuaternion fromRotation, Sombrero::FastQuaternion toRotation, float weight) {
        if (weight <= 0.0f)
        {
            return fromRotation;
        }
        if (weight >= 1.0f)
        {
            return toRotation;
        }
        return Sombrero::FastQuaternion::Lerp(fromRotation, toRotation, weight);
    }
}