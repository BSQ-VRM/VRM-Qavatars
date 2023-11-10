#pragma once

#include "main.hpp"

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Matrix4x4.hpp"

#include "vrm/VRMSpringBoneLogic.hpp"
#include "customTypes/VRMSpringBoneColliderGroup.hpp"

#include <map>

DECLARE_CLASS_CODEGEN(VRMQavatars, VRMSpringBone, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(ArrayW<UnityEngine::Transform*>, GetChildren, UnityEngine::Transform* parent);
    DECLARE_INSTANCE_METHOD(void, FixedUpdate);
    DECLARE_INSTANCE_METHOD(void, Reset);
    DECLARE_INSTANCE_METHOD(void, SetLocalRotationsIdentity);
    DECLARE_INSTANCE_METHOD(void, Setup, bool force = false);
    DECLARE_INSTANCE_METHOD(void, SetupRecursive, UnityEngine::Transform* center, UnityEngine::Transform* parent);
    DECLARE_INSTANCE_METHOD(void, UpdateProcess, float deltaTime);

    DECLARE_INSTANCE_FIELD(UnityEngine::Transform*, center);
    DECLARE_INSTANCE_FIELD(StringW, comment);
    DECLARE_INSTANCE_FIELD(float, dragForce);
    DECLARE_INSTANCE_FIELD(UnityEngine::Vector3, gravityDir);
    DECLARE_INSTANCE_FIELD(float, gravityPower);
    DECLARE_INSTANCE_FIELD(float, hitRadius);
    DECLARE_INSTANCE_FIELD(float, stiffnessForce);
    DECLARE_INSTANCE_FIELD(ArrayW<UnityEngine::Transform*>, rootBones);

    public:
    std::vector<VRMQavatars::VRMSpringBoneLogic*> verlet;
    std::vector<VRMQavatars::VRMSpringBoneColliderGroup*> colliderGroups;
    std::map<UnityEngine::Transform*, UnityEngine::Quaternion> initialLocalRotationMap;

)