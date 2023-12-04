#pragma once

#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Transform.hpp"

struct SphereCollider
{
    SphereCollider(const Sombrero::FastVector3 offset, const float radius)
    {
        this->offset = offset;
        this->radius = radius;
    }
    Sombrero::FastVector3 offset;
    float radius;
};

struct SphereColliderLogic
{
    Sombrero::FastVector3 Position;
    float Radius;

    SphereColliderLogic(UnityEngine::Transform* transform, const SphereCollider collider)
    {
        Position = transform->TransformPoint(collider.offset);
        const auto ls = transform->get_lossyScale();
        const auto scale = std::max(std::max(ls.x, ls.y), ls.z);
        Radius = scale * collider.radius;
    }
};