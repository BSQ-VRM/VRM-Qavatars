#pragma once
#include "sombrero/shared/FastVector3.hpp"
#include "sombrero/shared/FastQuaternion.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

namespace VRMQavatars::FinalIK {
    class VirtualBone {
    public:
        ~VirtualBone() = default;
        VirtualBone(Sombrero::FastVector3 position, Sombrero::FastQuaternion rotation) { Read(position, rotation); };

        void Read(Sombrero::FastVector3 position, Sombrero::FastQuaternion rotation);
        static void SwingRotation(std::vector<VirtualBone*> bones, int index, Sombrero::FastVector3 swingTarget, float weight = 1.0f);
        static float PreSolve(std::vector<VirtualBone*>& bones);
        static void RotateAroundPoint(std::vector<VirtualBone*> bones, int index, Sombrero::FastVector3 point, Sombrero::FastQuaternion rotation);
        static void RotateBy(std::vector<VirtualBone*> bones, Sombrero::FastQuaternion rotation);
        static void SolveTrigonometric(std::vector<VirtualBone*> bones, int first, int second, int third, Sombrero::FastVector3 targetPosition, Sombrero::FastVector3 bendNormal, float weight);
        static Sombrero::FastVector3 GetDirectionToBendPoint(Sombrero::FastVector3 direction, float directionMag, Sombrero::FastVector3 bendDirection, float sqrMag1, float sqrMag2);
        static void SolveFABRIK(std::vector<VirtualBone*> bones, Sombrero::FastVector3 startPosition, Sombrero::FastVector3 targetPosition, float weight, float minNormalizedTargetDistance, int iterations, float length, Sombrero::FastVector3 startOffset);
        static Sombrero::FastVector3 SolveFABRIKJoint(Sombrero::FastVector3 pos1, Sombrero::FastVector3 pos2, float length);

        Sombrero::FastVector3 readPosition;
        Sombrero::FastQuaternion readRotation;
        Sombrero::FastVector3 solverPosition;
        Sombrero::FastQuaternion solverRotation;
        float length;
        float sqrMag;
        Sombrero::FastVector3 axis;
    };
}