#pragma once

#include "sombrero/shared/FastVector3.hpp"
#include "sombrero/shared/FastQuaternion.hpp"

#include "UnityEngine/Transform.hpp"

namespace VRMQavatars::FinalIK {
    class Point {
    public:
        virtual ~Point() = default;
        Point() = default;
        explicit Point(UnityEngine::Transform* transform, float weight = 1.0f) : transform(transform), weight(weight) {};

        void FixTransforms();
        void StoreDefaultLocalState();
        void UpdateSolverLocalPosition();
        void UpdateSolverLocalState();
        void UpdateSolverPosition();
        void UpdateSolverState();

        Sombrero::FastVector3 defaultLocalPosition = Sombrero::FastVector3::zero();
        Sombrero::FastQuaternion defaultLocalRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 solverPosition = Sombrero::FastVector3::zero();
        Sombrero::FastQuaternion solverRotation = Sombrero::FastQuaternion::identity();
        UnityEngine::Transform* transform = nullptr;
        float weight = 1.0f;
    };
}