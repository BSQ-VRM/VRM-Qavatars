#include "customTypes/VRMSpringBone.hpp"

#include <sombrero/shared/FastQuaternion.hpp>

DEFINE_TYPE(VRMQavatars, VRMSpringBone);

namespace VRMQavatars
{
	void VRMSpringBone::Awake()
	{
		Setup(false);
	}

	ArrayW<UnityEngine::Transform*> VRMSpringBone::GetChildren(UnityEngine::Transform* parent)
	{
		ArrayW<UnityEngine::Transform*> result = ArrayW<UnityEngine::Transform*>(parent->get_childCount());
		int num;
		for (int i = 0; i < parent->get_childCount(); i = num)
		{
			result[i] = parent->GetChild(i);
			num = i + 1;
		}
		return result;
	}

	void VRMSpringBone::FixedUpdate()
	{
		UpdateProcess(UnityEngine::Time::get_fixedDeltaTime());
	}

	void VRMSpringBone::Reset()
	{
		verlet.clear();
	}

	void VRMSpringBone::SetLocalRotationsIdentity()
	{
		for(auto& logic : verlet)
		{
			logic->trans->set_localRotation(Sombrero::FastQuaternion::get_identity());
		}
	}

	void VRMSpringBone::Setup(bool force)
	{
		if(rootBones)
		{
			if (force || this->initialLocalRotationMap.size() == 0)
			{
				initialLocalRotationMap = std::map<UnityEngine::Transform*, Sombrero::FastQuaternion>();
			}
			else
			{
				for (auto& pair : initialLocalRotationMap)
				{
					pair.first->set_localRotation(pair.second);
				}
				initialLocalRotationMap.clear();
			}
			verlet.clear();
			for (auto& boneTrans : rootBones)
			{
				if(boneTrans != nullptr)
				{
					for (auto& child : boneTrans->GetComponentsInChildren<UnityEngine::Transform*>(true))
					{
						initialLocalRotationMap[child] = child->get_localRotation();
					}
					SetupRecursive(center, boneTrans);
				}
			}
		}
	}

	void VRMSpringBone::SetupRecursive(UnityEngine::Transform* center, UnityEngine::Transform* parent)
	{
		Sombrero::FastVector3 vector;
		Sombrero::FastVector3 vector2;
		if (parent->get_childCount() == 0)
		{
			const Sombrero::FastVector3 vector3 = parent->get_position() - parent->get_parent()->get_position();
			const float lossyScale = std::max(std::max(parent->get_lossyScale().x, parent->get_lossyScale().y), parent->get_lossyScale().z);
			const Sombrero::FastVector3 vector4 = parent->get_position() + vector3.get_normalized() * 0.07f * lossyScale;
			vector = parent->get_worldToLocalMatrix().MultiplyPoint(vector4);
			vector2 = parent->get_lossyScale();
		}
		else
		{
			UnityEngine::Transform* transform = GetChildren(parent).First();
			vector = transform->get_localPosition();
			vector2 = transform->get_lossyScale();
		}
		verlet.push_back(new VRMSpringBoneLogic(center, parent, Sombrero::FastVector3(vector.x * vector2.x, vector.y * vector2.y, vector.z * vector2.z)));
		for (size_t i = 0; i < parent->get_childCount(); i++)
		{
			const auto child = parent->GetChild(i);
			SetupRecursive(center, child);
		}
	}

	void VRMSpringBone::UpdateProcess(const float deltaTime)
	{
		if (verlet.size() == 0)
		{
			if (rootBones.Length() == 0)
			{
				return;
			}
			Setup(false);
		}
		std::vector<SphereColliderLogic> colliders;
		for (auto vrmspringBoneColliderGroup : colliderGroups)
		{
			if (vrmspringBoneColliderGroup != nullptr)
			{
				for (auto sphereCollider : vrmspringBoneColliderGroup->colliders)
				{
					colliders.push_back(SphereColliderLogic(vrmspringBoneColliderGroup->get_transform(), sphereCollider));
				}
			}
		}
		float num = stiffnessForce * deltaTime;
		Sombrero::FastVector3 vector = gravityDir * (gravityPower * deltaTime);
		for (auto& logic : verlet)
		{
			logic->SetRadius(hitRadius);
			logic->Update(center, num, dragForce, vector, colliders);
		}
	}
}