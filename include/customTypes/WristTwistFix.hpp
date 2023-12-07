#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>
#include <sombrero/shared/FastVector3.hpp>

#include "main.hpp"

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Mathf.hpp"

#include "RootMotion/FinalIK/IKSolver_UpdateDelegate.hpp"
#include "RootMotion/FinalIK/VRIK.hpp"
#include "RootMotion/FinalIK/VRIK_References.hpp"

#include "custom-types/shared/delegate.hpp"

namespace VRMQavatars
{
    class FixData
    {
        public:
        FixData(UnityEngine::Transform* parent, UnityEngine::Transform* target, UnityEngine::Transform* child)
        {
            if (target == nullptr || child == nullptr)
			{
				return;
			}
			this->parent = parent;
			this->target = target;
			this->child = child;
			this->twistAxis = target->InverseTransformDirection(child->get_position() - target->get_position());
			this->axis = UnityEngine::Vector3(this->twistAxis.y, this->twistAxis.z, this->twistAxis.x);
			this->targetAxisWorld = target->get_rotation() * this->axis;
			this->axisRelativeToTargetDefault = UnityEngine::Quaternion::Inverse(target->get_rotation()) * this->targetAxisWorld;
			if (parent != nullptr)
			{
				this->axisRelativeToParentDefault = UnityEngine::Quaternion::Inverse(parent->get_rotation()) * this->targetAxisWorld;
			}
			this->axisRelativeToChildDefault = UnityEngine::Quaternion::Inverse(child->get_rotation()) * this->targetAxisWorld;
        }

        Sombrero::FastVector3 twistAxis = UnityEngine::Vector3::get_right();

        Sombrero::FastVector3 axis = UnityEngine::Vector3::get_forward();

		Sombrero::FastVector3 axisRelativeToParentDefault;

		Sombrero::FastVector3 axisRelativeToChildDefault;

		Sombrero::FastVector3 axisRelativeToTargetDefault;

        UnityEngine::Transform* parent;

        UnityEngine::Transform* target;

        UnityEngine::Transform* child;

		Sombrero::FastVector3 targetAxisWorld;
    };

	class WristTwistFix : public UnityEngine::MonoBehaviour
	{
		CP_SDK_IL2CPP_INHERIT("VRMQavatars", WristTwistFix, UnityEngine::MonoBehaviour);
		CP_SDK_IL2CPP_DECLARE_CTOR(WristTwistFix);
		CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(WristTwistFix);

		DECLARE_INSTANCE_METHOD(void, SetVRIK, RootMotion::FinalIK::VRIK* vrik);
		DECLARE_INSTANCE_METHOD(void, OnPostUpdate);
		float Dampen(float v, float min, float max);

		float elbowWeight = 0.5f;
		float shoulderWeight = 0.2f;
		float wristWeight = 0.4f;

		RootMotion::FinalIK::VRIK* _vrik;
		RootMotion::FinalIK::IKSolver::UpdateDelegate* postUpdateDelegate;
	public:
		FixData* _leftWristFixData;
		FixData* _rightWristFixData;
		FixData* _leftShoulderFixData;
		FixData* _rightShoulderFixData;
		FixData* _leftElbowFixData;
		FixData* _rightElbowFixData;
		void FixAxis(FixData* fixData, float weight, float dampen);
	};
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::WristTwistFix);