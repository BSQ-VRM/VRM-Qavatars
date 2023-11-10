#pragma once

#include "main.hpp"

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"

#include "vrm/vrmColliders.hpp"

#include <map>

DECLARE_CLASS_CODEGEN(VRMQavatars, VRMSpringBoneColliderGroup, UnityEngine::MonoBehaviour,
    public:
    std::vector<SphereCollider> colliders;
)