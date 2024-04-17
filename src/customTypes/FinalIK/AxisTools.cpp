#pragma once 

#include "customTypes/FinalIK/AxisTools.hpp"

namespace VRMQavatars::FinalIK::AxisTools {
    Sombrero::FastVector3 ToVector3(Axis axis) {
        if (axis == Axis::X)
        {
            return Sombrero::FastVector3::right();
        }
        if (axis == Axis::Y)
        {
            return Sombrero::FastVector3::up();
        }
        return Sombrero::FastVector3::forward();
    }

    Axis GetAxisToDirection(UnityEngine::Transform* t, Sombrero::FastVector3 direction) {
        Sombrero::FastVector3 axisVectorToDirection = GetAxisVectorToDirection(t, direction);
        if (axisVectorToDirection == Sombrero::FastVector3::right())
        {
            return Axis::X;
        }
        if (axisVectorToDirection == Sombrero::FastVector3::up())
        {
            return Axis::Y;
        }
        return Axis::Z;
    }

    Sombrero::FastVector3 GetAxisVectorToDirection(UnityEngine::Transform* t, Sombrero::FastVector3 direction) {
        return GetAxisVectorToDirection(t->rotation, direction);
    }

    Sombrero::FastVector3 GetAxisVectorToDirection(Sombrero::FastQuaternion r, Sombrero::FastVector3 direction) {
        direction = direction.normalized;
        Sombrero::FastVector3 vector = Sombrero::FastVector3::right();
        float num =  std::abs(Sombrero::FastVector3::Dot(Sombrero::FastVector3::Normalize(r * Sombrero::FastVector3::right()), direction));
        float num2 = std::abs(Sombrero::FastVector3::Dot(Sombrero::FastVector3::Normalize(r * Sombrero::FastVector3::up()), direction));
        if (num2 > num)
        {
            vector = Sombrero::FastVector3::up();
        }
        float num3 = std::abs(Sombrero::FastVector3::Dot(Sombrero::FastVector3::Normalize(r * Sombrero::FastVector3::forward()), direction));
        if (num3 > num && num3 > num2)
        {
            vector = Sombrero::FastVector3::forward();
        }
        return vector;
    }
}