#include "customTypes/VRMSpringBone.hpp"

DEFINE_TYPE(VRMQavatars, VRMSpringBone);

void VRMQavatars::VRMSpringBone::Awake()
{
    Setup(false);
}

ArrayW<UnityEngine::Transform*> VRMQavatars::VRMSpringBone::GetChildren(UnityEngine::Transform* parent)
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

void VRMQavatars::VRMSpringBone::FixedUpdate()
{
    UpdateProcess(UnityEngine::Time::get_fixedDeltaTime());
}

void VRMQavatars::VRMSpringBone::Reset()
{
    verlet.clear();
}

void VRMQavatars::VRMSpringBone::SetLocalRotationsIdentity()
{
    for(auto& logic : verlet)
    {
        logic->trans->set_localRotation(UnityEngine::Quaternion::get_identity());
    }
}

void VRMQavatars::VRMSpringBone::Setup(bool force)
{
    getLogger().info("x1");
    if(rootBones)
    {
        getLogger().info("x2");
        if (force || this->initialLocalRotationMap.size() == 0)
        {
            getLogger().info("x3");
            initialLocalRotationMap = std::map<UnityEngine::Transform*, UnityEngine::Quaternion>();
        }
        else
        {
            getLogger().info("x4");
            for (auto& pair : initialLocalRotationMap)
            {
                getLogger().info("x5");
                pair.first->set_localRotation(pair.second);
            }
            getLogger().info("x6");
            initialLocalRotationMap.clear();
        }
        getLogger().info("x7");
        verlet.clear();
        getLogger().info("x8");
        for (auto& boneTrans : rootBones)
        {
            getLogger().info("x9");
            if(boneTrans != nullptr)
            {
                getLogger().info("x10");
                for (auto& child : boneTrans->GetComponentsInChildren<UnityEngine::Transform*>(true))
                {
                    getLogger().info("x11");
                    initialLocalRotationMap[child] = child->get_localRotation();
                }
                getLogger().info("x12");
                SetupRecursive(center, boneTrans);
            }
            getLogger().info("x13");
        }
        getLogger().info("x14");
    }
}

void VRMQavatars::VRMSpringBone::SetupRecursive(UnityEngine::Transform* center, UnityEngine::Transform* parent)
{
    UnityEngine::Vector3 vector;
	UnityEngine::Vector3 vector2;
	if (parent->get_childCount() == 0)
	{
		UnityEngine::Vector3 vector3 = parent->get_position() - parent->get_parent()->get_position();
        float lossyScale = std::max(std::max(parent->get_lossyScale().x, parent->get_lossyScale().y), parent->get_lossyScale().z);
		UnityEngine::Vector3 vector4 = parent->get_position() + vector3.get_normalized() * 0.07f * lossyScale;
		vector = parent->get_worldToLocalMatrix().MultiplyPoint(vector4);
		vector2 = parent->get_lossyScale();
	}
	else
	{
		UnityEngine::Transform* transform = GetChildren(parent).First();
		vector = transform->get_localPosition();
		vector2 = transform->get_lossyScale();
	}
	verlet.push_back(new VRMQavatars::VRMSpringBoneLogic(center, parent, UnityEngine::Vector3(vector.x * vector2.x, vector.y * vector2.y, vector.z * vector2.z)));
    for (size_t i = 0; i < parent->get_childCount(); i++)
    {
        auto child = parent->GetChild(i);
        SetupRecursive(center, child);
    }
}

void VRMQavatars::VRMSpringBone::UpdateProcess(float deltaTime)
{
    if (verlet.size() == 0)
	{
		if (rootBones.Length() == 0)
		{
			return;
		}
		Setup(false);
	}
	/*this.m_colliders.Clear();
	if (this.ColliderGroups != null)
	{
		foreach (VRMSpringBoneColliderGroup vrmspringBoneColliderGroup in this.ColliderGroups)
		{
			if (vrmspringBoneColliderGroup != null)
			{
				foreach (VRMSpringBoneColliderGroup.SphereCollider sphereCollider in vrmspringBoneColliderGroup.Colliders)
				{
					this.m_colliders.Add(new VRMSpringBone.SphereCollider(vrmspringBoneColliderGroup.transform, sphereCollider));
				}
			}
		}
	}*/
	float num = stiffnessForce * deltaTime;
	UnityEngine::Vector3 vector = gravityDir * (gravityPower * deltaTime);
	for (auto& logic : verlet)
	{
		logic->SetRadius(hitRadius);
		logic->Update(center, num, dragForce, vector);
	}
}