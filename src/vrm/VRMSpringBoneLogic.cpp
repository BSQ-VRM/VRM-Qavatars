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
    UnityEngine::Vector3 vector = ((center != nullptr) ? center->TransformPoint(currentTail) : currentTail);
	UnityEngine::Vector3 vector2 = ((center != nullptr) ? center->TransformPoint(prevTail) : prevTail);
	UnityEngine::Vector3 vector3 = vector + (vector - vector2) * (1.0f - dragForce) + GetParentRotation() * localRotation * boneAxis * stiffnessForce + external;
	UnityEngine::Vector3 position = trans->get_position();
	vector3 = position + (vector3 - position).get_normalized() * length;
	vector3 = Collision(colliders, vector3);
	prevTail = ((center != nullptr) ? center->InverseTransformPoint(vector) : vector);
	currentTail = ((center != nullptr) ? center->InverseTransformPoint(vector3) : vector3);
	trans->set_rotation(ApplyRotation(vector3));
}

UnityEngine::Vector3 VRMQavatars::VRMSpringBoneLogic::Collision(std::vector<SphereColliderLogic> colliders, UnityEngine::Vector3 nextTail)
{
	UnityEngine::Vector3 ret = nextTail;
	for (int i = 0; i < colliders.size(); i++)
	{
		auto collider = colliders[i];
		auto r = radius * collider.Radius;
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