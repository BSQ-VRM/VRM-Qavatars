#pragma once

#include <sombrero/shared/FastVector3.hpp>

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
        VRMSpringBoneLogic(UnityEngine::Transform* center, UnityEngine::Transform* transform, Sombrero::FastVector3 localChildPosition):
            radius(0)
        {
            trans = transform;
            Sombrero::FastVector3 vector = trans->TransformPoint(localChildPosition);
            currentTail = center != nullptr ? center->InverseTransformPoint(vector) : vector;
            prevTail = currentTail;
            localRotation = transform->get_localRotation();
            boneAxis = localChildPosition.get_normalized();
            length = localChildPosition.get_magnitude();
        }

        UnityEngine::Quaternion ApplyRotation(Sombrero::FastVector3 nextTail);
        void SetRadius(float radius);
        void Update(UnityEngine::Transform* center, float stiffnessForce, float dragForce, Sombrero::FastVector3 external, const std::vector<SphereColliderLogic>& colliders);
        Sombrero::FastVector3 Collision(const std::vector<SphereColliderLogic>& colliders, Sombrero::FastVector3 nextTail);

        Sombrero::FastVector3 GetTail();
        UnityEngine::Quaternion GetParentRotation();
        UnityEngine::Quaternion localRotation;
        Sombrero::FastVector3 boneAxis;
        Sombrero::FastVector3 currentTail;
        float length;
        Sombrero::FastVector3 localDir;
        Sombrero::FastVector3 prevTail;
        float radius;
        UnityEngine::Transform* trans;
    };
};