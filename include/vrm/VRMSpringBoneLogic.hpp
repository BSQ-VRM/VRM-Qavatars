#pragma once

#include "main.hpp"

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Matrix4x4.hpp"

#include "RootMotion/FinalIK/VRIK.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Arm.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Spine.hpp"

#include "GlobalNamespace/OVRPlugin.hpp"
#include "GlobalNamespace/OVRPlugin_Node.hpp"
#include "GlobalNamespace/OVRPlugin_Step.hpp"
#include "GlobalNamespace/OVRPose.hpp"

#include "vrm/vrmColliders.hpp"

namespace VRMQavatars
{
    class VRMSpringBoneLogic
    {
        public:
        VRMSpringBoneLogic(UnityEngine::Transform* center, UnityEngine::Transform* transform, UnityEngine::Vector3 localChildPosition)
        {
            trans = transform;
            UnityEngine::Vector3 vector = trans->TransformPoint(localChildPosition);
            currentTail = ((center != nullptr) ? center->InverseTransformPoint(vector) : vector);
            prevTail = currentTail;
            localRotation = transform->get_localRotation();
            boneAxis = localChildPosition.get_normalized();
            length = localChildPosition.get_magnitude();
        }
        UnityEngine::Quaternion ApplyRotation(UnityEngine::Vector3 nextTail);
        void SetRadius(float radius);
        void Update(UnityEngine::Transform* center, float stiffnessForce, float dragForce, UnityEngine::Vector3 external, std::vector<SphereColliderLogic> colliders);
        UnityEngine::Vector3 Collision(std::vector<SphereColliderLogic> colliders, UnityEngine::Vector3 nextTail);

        UnityEngine::Vector3 GetTail();
        UnityEngine::Quaternion GetParentRotation();
        UnityEngine::Quaternion localRotation;
        UnityEngine::Vector3 boneAxis;
        UnityEngine::Vector3 currentTail;
        float length;
        UnityEngine::Vector3 localDir;
        UnityEngine::Vector3 prevTail;
        float radius;
        UnityEngine::Transform* trans;
    };
};