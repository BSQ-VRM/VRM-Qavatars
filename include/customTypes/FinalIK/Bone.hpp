#pragma once

#include "Point.hpp"

#include "sombrero/shared/FastVector3.hpp"

#include "UnityEngine/Transform.hpp"

namespace VRMQavatars::FinalIK {
    class Bone : public Point {
    public:
        virtual ~Bone() = default;
        Bone();
        Bone(UnityEngine::Transform* transform) : Bone(transform, 1.0f) {};
        Bone(UnityEngine::Transform* transform, float weight = 1.0f) : Point(transform, weight) {};

        void SetToSolverPosition();
        void Swing(Sombrero::FastVector3 swingTarget, float weight = 1.0f);
        void Swing2D(Sombrero::FastVector3 swingTarget, float weight = 1.0f);

        Sombrero::FastVector3 axis;
        bool isLimited;
        float length;
        float sqrMag;
    };

    static void Bone_SolverSwing(std::vector<Bone*> bones, int index, Sombrero::FastVector3 swingTarget, float weight = 1.0f);
}