#pragma once

#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Transform.hpp"

struct SphereCollider
{
    SphereCollider(UnityEngine::Vector3 offset, float radius)
    {
        this->offset = offset;
        this->radius = radius;
    }
    UnityEngine::Vector3 offset;
    float radius;
};

struct SphereColliderLogic
{
    UnityEngine::Vector3 Position;
    float Radius;

    SphereColliderLogic(UnityEngine::Transform* transform, SphereCollider collider)
    {
        Position = transform->TransformPoint(collider.offset);
        auto ls = transform->get_lossyScale();
        auto scale = std::max(std::max(ls.x, ls.y), ls.z);
        Radius = scale * collider.radius;
    }
};