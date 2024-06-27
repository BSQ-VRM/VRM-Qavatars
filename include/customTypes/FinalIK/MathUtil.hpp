#pragma once
#include "sombrero/shared/FastVector3.hpp"

namespace VRMQavatars::FinalIK {

    static Sombrero::FastVector3 ClampMagnitude(Sombrero::FastVector3 vector, float maxLength)
    {
        float sqrMagnitude = vector.sqrMagnitude();
        bool flag = sqrMagnitude > maxLength * maxLength;
        Sombrero::FastVector3 vector2;
        if (flag)
        {
            float num = (float)std::sqrt((double)sqrMagnitude);
            float num2 = vector.x / num;
            float num3 = vector.y / num;
            float num4 = vector.z / num;
            vector2 = Sombrero::FastVector3(num2 * maxLength, num3 * maxLength, num4 * maxLength);
        }
        else
        {
            vector2 = vector;
        }
        return vector2;
    }

    static float DeltaAngle(float current, float target)
    {  
        float t = target - current;
        float length = 360.0f;
        float num = std::clamp(t - std::floor(t / length) * length, 0.0f, length);
        bool flag = num > 180.0f;
        if (flag)
        {
            num -= 360.0f;
        }
        return num;
    }
}