#pragma once
#include "sombrero/shared/FastVector3.hpp"

namespace VRMQavatars::FinalIK::V3Tools {
    Sombrero::FastVector3 Lerp(Sombrero::FastVector3 fromVector, Sombrero::FastVector3 toVector, float weight);
    Sombrero::FastVector3 ExtractVertical(Sombrero::FastVector3 v, Sombrero::FastVector3 verticalAxis, float weight);
    Sombrero::FastVector3 ExtractHorizontal(Sombrero::FastVector3 v, Sombrero::FastVector3 normal, float weight);
    Sombrero::FastVector3 PointToPlane(Sombrero::FastVector3 point, Sombrero::FastVector3 planePosition, Sombrero::FastVector3 planeNormal);
}