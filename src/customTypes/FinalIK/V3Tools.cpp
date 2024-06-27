#include "customTypes/FinalIK/V3Tools.hpp"

namespace VRMQavatars::FinalIK::V3Tools {
    Sombrero::FastVector3 Lerp(Sombrero::FastVector3 fromVector, Sombrero::FastVector3 toVector, float weight) {
        if (weight <= 0.0f)
        {
            return fromVector;
        }
        if (weight >= 1.0f)
        {
            return toVector;
        }
        return Sombrero::FastVector3::Lerp(fromVector, toVector, weight);
    }

    Sombrero::FastVector3 ExtractVertical(Sombrero::FastVector3 v, Sombrero::FastVector3 verticalAxis, float weight) {
        if (weight == 0.0f)
        {
            return Sombrero::FastVector3::zero();
        }
        Sombrero::FastVector3 proj = Sombrero::FastVector3::Project(v, verticalAxis);
        return proj * weight;
    }

    Sombrero::FastVector3 ExtractHorizontal(Sombrero::FastVector3 v, Sombrero::FastVector3 normal, float weight) {
        if (weight == 0.0f)
        {
            return Sombrero::FastVector3::zero();
        }
        UnityEngine::Vector3 normal2 = normal;
        UnityEngine::Vector3 vector = v;
        Sombrero::FastVector3::OrthoNormalize(byref(normal2), byref(vector));
        Sombrero::FastVector3 proj = Sombrero::FastVector3::Project(v, vector);
        return proj * weight;
    }

    Sombrero::FastVector3 PointToPlane(Sombrero::FastVector3 point, Sombrero::FastVector3 planePosition, Sombrero::FastVector3 planeNormal) {
        if (planeNormal.Equals(Sombrero::FastVector3::up()))
        {
            return {point.x, planePosition.y, point.z}; 
        }
        UnityEngine::Vector3 vector = point - planePosition;
        UnityEngine::Vector3 vector2 = planeNormal;
        Sombrero::FastVector3::OrthoNormalize(byref(vector2), byref(vector));
        return planePosition + Sombrero::FastVector3::Project(point - planePosition, vector);
    }
}