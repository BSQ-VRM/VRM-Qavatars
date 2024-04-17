#pragma once

#include "Point.hpp"

#include "sombrero/shared/FastVector3.hpp"

#include "UnityEngine/Transform.hpp"

namespace VRMQavatars::FinalIK {
    class Node : public Point {
    public:
        virtual ~Node() = default;
        Node();
        Node(UnityEngine::Transform* transform) : Node(transform, 1.0f) {};
        Node(UnityEngine::Transform* transform, float weight = 1.0f) : Point(transform, weight) {};

        float effectorPositionWeight;
        float effectorRotationWeight;
        float length;
        Sombrero::FastVector3 offset;
    };
}