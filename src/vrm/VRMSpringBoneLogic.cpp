#include "vrm/VRMSpringBoneLogic.hpp"

#include <sombrero/shared/FastVector3.hpp>

namespace VRMQavatars
{
	Sombrero::FastQuaternion VRMSpringBoneLogic::ApplyRotation(const Sombrero::FastVector3 nextTail)
	{
		const Sombrero::FastQuaternion quaternion = GetParentRotation() * localRotation;
		return Sombrero::FastQuaternion(UnityEngine::Quaternion::FromToRotation(quaternion * boneAxis, nextTail - trans->get_position())) * quaternion;
	}

	void VRMSpringBoneLogic::SetRadius(const float radius)
	{
		const float lossyScale = std::max(std::max(trans->get_lossyScale().x, trans->get_lossyScale().y), trans->get_lossyScale().z);
		this->radius = radius * lossyScale;
	}

	void VRMSpringBoneLogic::Update(UnityEngine::Transform* center, const float stiffnessForce, const float dragForce, const Sombrero::FastVector3 external, const std::vector<SphereColliderLogic>& colliders)
	{
		Sombrero::FastVector3 currentTailVec = center != nullptr ? center->TransformPoint(currentTail) : currentTail;
		const Sombrero::FastVector3 prevTailVec = center != nullptr ? center->TransformPoint(prevTail) : prevTail;
		Sombrero::FastVector3 nextTailVec = currentTailVec + (currentTailVec - prevTailVec) * (1.0f - dragForce) + GetParentRotation() * localRotation * boneAxis * stiffnessForce + external;
		const Sombrero::FastVector3 position = trans->get_position();
		nextTailVec = position + (nextTailVec - position).get_normalized() * length;
		nextTailVec = Collision(colliders, nextTailVec);
		prevTail = center != nullptr ? center->InverseTransformPoint(currentTailVec) : currentTailVec;
		currentTail = center != nullptr ? center->InverseTransformPoint(nextTailVec) : nextTailVec;
		trans->set_rotation(ApplyRotation(nextTailVec));
	}

	float SqrMagnitude(Sombrero::FastVector3 vector) {
		return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
	}

	Sombrero::FastVector3 VRMSpringBoneLogic::Collision(const std::vector<SphereColliderLogic>& colliders, const Sombrero::FastVector3 nextTail)
	{
		UnityEngine::Vector3 ret = nextTail;
		for (int i = 0; i < colliders.size(); i++)
		{
			auto collider = colliders[i];
			const auto r = radius + collider.Radius;
			if (SqrMagnitude(nextTail - collider.Position) <= (r * r))
			{
				// ヒット。Colliderの半径方向に押し出す
				auto normal = (nextTail - collider.Position).get_normalized();
				auto posFromCollider = collider.Position + normal * (radius + collider.Radius);
				// 長さをboneLengthに強制
				ret = Sombrero::FastVector3(trans->get_position()) + (posFromCollider - trans->get_position()).get_normalized() * length;
			}
		}
		return ret;
	}

	Sombrero::FastVector3 VRMSpringBoneLogic::GetTail()
	{
		return trans->get_localToWorldMatrix().MultiplyPoint(boneAxis * length);
	}

	Sombrero::FastQuaternion VRMSpringBoneLogic::GetParentRotation()
	{
		if (trans->get_parent() == nullptr)
		{
			return Sombrero::FastQuaternion::get_identity();
		}
		return trans->get_parent()->get_rotation();
	}
}