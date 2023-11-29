#include "vrm/VRMSpringBoneLogic.hpp"

UnityEngine::Quaternion VRMQavatars::VRMSpringBoneLogic::ApplyRotation(UnityEngine::Vector3 nextTail)
{
    UnityEngine::Quaternion quaternion = GetParentRotation() * localRotation;
	return UnityEngine::Quaternion::FromToRotation(quaternion * boneAxis, nextTail - trans->get_position()) * quaternion;
}

void VRMQavatars::VRMSpringBoneLogic::SetRadius(float radius)
{
    float lossyScale = std::max(std::max(trans->get_lossyScale().x, trans->get_lossyScale().y), trans->get_lossyScale().z);
    this->radius = radius * lossyScale;
}

void VRMQavatars::VRMSpringBoneLogic::Update(UnityEngine::Transform* center, float stiffnessForce, float dragForce, UnityEngine::Vector3 external, std::vector<SphereColliderLogic> colliders)
{
    UnityEngine::Vector3 currentTailVec = ((center != nullptr) ? center->TransformPoint(currentTail) : currentTail);
	UnityEngine::Vector3 prevTailVec = ((center != nullptr) ? center->TransformPoint(prevTail) : prevTail);
	UnityEngine::Vector3 nextTailVec = currentTailVec + (currentTailVec - prevTailVec) * (1.0f - dragForce) + GetParentRotation() * localRotation * boneAxis * stiffnessForce + external;
	UnityEngine::Vector3 position = trans->get_position();
	nextTailVec = position + (nextTailVec - position).get_normalized() * length;
	nextTailVec = Collision(colliders, nextTailVec);
	prevTail = ((center != nullptr) ? center->InverseTransformPoint(currentTailVec) : currentTailVec);
	currentTail = ((center != nullptr) ? center->InverseTransformPoint(nextTailVec) : nextTailVec);
	trans->set_rotation(ApplyRotation(nextTailVec));
}

UnityEngine::Vector3 VRMQavatars::VRMSpringBoneLogic::Collision(std::vector<SphereColliderLogic> colliders, UnityEngine::Vector3 nextTail)
{
	UnityEngine::Vector3 ret = nextTail;
	for (int i = 0; i < colliders.size(); i++)
	{
		auto collider = colliders[i];
		auto r = radius + collider.Radius;
		if (UnityEngine::Vector3::SqrMagnitude(nextTail - collider.Position) <= (r * r))
        {
            // ヒット。Colliderの半径方向に押し出す
            auto normal = (nextTail - collider.Position).get_normalized();
            auto posFromCollider = collider.Position + normal * (radius + collider.Radius);
            // 長さをboneLengthに強制
            ret = trans->get_position() + (posFromCollider - trans->get_position()).get_normalized() * length;
        }
	}
	return ret;
}

UnityEngine::Vector3 VRMQavatars::VRMSpringBoneLogic::GetTail()
{
    return trans->get_localToWorldMatrix().MultiplyPoint(boneAxis * length);
}

UnityEngine::Quaternion VRMQavatars::VRMSpringBoneLogic::GetParentRotation()
{
    if (trans->get_parent() == nullptr)
	{
		return UnityEngine::Quaternion::get_identity();
	}
	return trans->get_parent()->get_rotation();
}