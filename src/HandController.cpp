#include "HandController.hpp"
#include <sstream>
#include "UnityEngine/HumanBodyBones.hpp"
#include "UnityEngine/Transform.hpp"

std::vector<UnityEngine::HumanBodyBones> fingerBones = {
		UnityEngine::HumanBodyBones::LeftLittleDistal,
		UnityEngine::HumanBodyBones::LeftLittleIntermediate,
		UnityEngine::HumanBodyBones::LeftLittleProximal,
		UnityEngine::HumanBodyBones::LeftRingDistal,
		UnityEngine::HumanBodyBones::LeftRingIntermediate,
		UnityEngine::HumanBodyBones::LeftRingProximal,
		UnityEngine::HumanBodyBones::LeftMiddleDistal,
		UnityEngine::HumanBodyBones::LeftMiddleIntermediate,
		UnityEngine::HumanBodyBones::LeftMiddleProximal,
		UnityEngine::HumanBodyBones::LeftIndexDistal,
		UnityEngine::HumanBodyBones::LeftIndexIntermediate,
		UnityEngine::HumanBodyBones::LeftIndexProximal,
		UnityEngine::HumanBodyBones::LeftThumbDistal,
		UnityEngine::HumanBodyBones::LeftThumbIntermediate,
		UnityEngine::HumanBodyBones::LeftThumbProximal,
		UnityEngine::HumanBodyBones::RightLittleDistal,
		UnityEngine::HumanBodyBones::RightLittleIntermediate,
		UnityEngine::HumanBodyBones::RightLittleProximal,
		UnityEngine::HumanBodyBones::RightRingDistal,
		UnityEngine::HumanBodyBones::RightRingIntermediate,
		UnityEngine::HumanBodyBones::RightRingProximal,
		UnityEngine::HumanBodyBones::RightMiddleDistal,
		UnityEngine::HumanBodyBones::RightMiddleIntermediate,
		UnityEngine::HumanBodyBones::RightMiddleProximal,
		UnityEngine::HumanBodyBones::RightIndexDistal,
		UnityEngine::HumanBodyBones::RightIndexIntermediate,
		UnityEngine::HumanBodyBones::RightIndexProximal,
		UnityEngine::HumanBodyBones::RightThumbDistal,
		UnityEngine::HumanBodyBones::RightThumbIntermediate,
		UnityEngine::HumanBodyBones::RightThumbProximal
	};

std::vector<std::string> SplitPose(std::string pose)
{
    std::vector<std::string> ret;
    std::string s;
    std::stringstream ss(pose);
    std::vector<std::string> v;
    while (getline(ss, s, ',')) {
        ret.push_back(s);
    }
    return ret;
}

std::vector<float> ParseRotations(std::vector<std::string> pose)
{
    std::vector<float> ret = std::vector<float>(pose.size());
    for (size_t i = 0; i < pose.size(); i++)
    {
        ret[i] = (float)stoi(pose[i]);
    }
    
    return ret;
}

void VRMQavatars::HandController::ApplyHandPose(UnityEngine::Animator* animator, std::string pose)
{
    auto array = SplitPose(pose);
	auto rotations = ParseRotations(array);

	auto angles = std::vector<UnityEngine::Vector3>(fingerBones.size());
	auto boneAngles = std::vector<UnityEngine::Vector3>(fingerBones.size());
	for (int i = 0; i < fingerBones.size(); i++)
	{
		boneAngles[i] = animator->GetBoneTransform(fingerBones[i])->get_localEulerAngles();
	}

	if (rotations.size() != 20)
	{
		return;
	}

	int num = fingerBones.size() / 2;

	if (true)
	{
		for (int i = 0; i < num; i += 3)
		{
			if (i >= 12)
			{
                //Bone 1
				auto bone1Angle = boneAngles[i + 2];
				float rot1 = rotations[i / 3 * 4 + 3];
				float rot2 = rotations[i / 3 * 4 + 2];
				angles[i + 2] = UnityEngine::Vector3(bone1Angle.x, bone1Angle.y - rot1, bone1Angle.z - rot2);

                //Bone 2
				auto bone2Angle = boneAngles[i + 1];
				float bone2Rot = -rotations[i / 3 * 4 + 1] / 90.0f;
				angles[i + 1] = UnityEngine::Vector3(bone2Angle.x, bone2Angle.y - bone2Rot * 38.0f, bone2Angle.z - bone2Rot * -15.0f);

                //Bone 3
				auto bone3Angle = boneAngles[i];
				float bone3Rot = -rotations[i / 3 * 4] / 90.0f;
				angles[i] = UnityEngine::Vector3(bone3Angle.x + bone3Rot * 34.0f, bone3Angle.y - bone3Rot * 56.0f, bone3Angle.z - bone3Rot * -7.0f);
			}
			else
			{
                //Bone 1
				auto bone1Angle = boneAngles[i + 2];
				float rot1 = rotations[i / 3 * 4 + 2];
				float rot2 = rotations[i / 3 * 4 + 3];
				angles[i + 2] = UnityEngine::Vector3(bone1Angle.x, bone1Angle.y - rot2, bone1Angle.z - rot1);

                //Bone 2
				auto bone2Angle = boneAngles[i + 1];
				auto bone1Rot = rotations[i / 3 * 4 + 1];
				angles[i + 1] = UnityEngine::Vector3(bone2Angle.x, bone2Angle.y, bone2Angle.z - bone1Rot);

                //Bone 3
				auto bone3Angle = boneAngles[i];
				auto bone3Rot = rotations[i / 3 * 4];
				angles[i] = UnityEngine::Vector3(bone3Angle.x, bone3Angle.y, bone3Angle.z - bone3Rot);
			}
		}
	}
	if (true)
	{
		for (int j = 0; j < num; j += 3)
		{
			if (j >= 12)
			{
                //Bone 1
				auto vector3 = boneAngles[j + num + 2];
				float num11 = -rotations[j / 3 * 4 + 2] / 90.0f;
				float num14 = rotations[j / 3 * 4 + 3];
				float num15 = rotations[j / 3 * 4 + 2];
				angles[j + num + 2] = UnityEngine::Vector3(vector3.x, vector3.y + num14, vector3.z + num15);

                //Bone 2
				vector3 = boneAngles[j + num + 1];
				float num16 = -rotations[j / 3 * 4 + 1] / 90.0f;
				angles[j + num + 1] = UnityEngine::Vector3(vector3.x + num16 * 38.0f, vector3.y + num16 * 38.0f, vector3.z + num16 * -15.0f);

                //Bone 3
				vector3 = boneAngles[j + num];
				float num17 = -rotations[j / 3 * 4] / 90.0f;
				angles[j + num] = UnityEngine::Vector3(vector3.x + num17 * 34.0f, vector3.y + num17 * 56.0f, vector3.z + num17 * -7.0f);
			}
			else
			{
                //Bone 1
				auto bone1Angle = boneAngles[j + num + 2];
				float rot1 = rotations[j / 3 * 4 + 2];
				float rot2 = rotations[j / 3 * 4 + 3];
				angles[j + num + 2] = UnityEngine::Vector3(bone1Angle.x, bone1Angle.y + rot2, bone1Angle.z + rot1);

                //Bone 2
				auto bone2Angle = boneAngles[j + num + 1];
				auto bone2Rot = rotations[j / 3 * 4 + 1];
				angles[j + num + 1] = UnityEngine::Vector3(bone2Angle.x, bone2Angle.y, bone2Angle.z + bone2Rot);

                //Bone 3
				auto bone3Angle = boneAngles[j + num];
				auto bone3Rot = rotations[j / 3 * 4];
				angles[j + num] = UnityEngine::Vector3(bone3Angle.x, bone3Angle.y, bone3Angle.z + bone3Rot);
			}
		}
	}

	for (int j = 0; j < num; j++)
	{
		animator->GetBoneTransform(fingerBones[j])->set_localEulerAngles(angles[j]);
	}

	for (int j = 0; j < num; j++)
	{
		animator->GetBoneTransform(fingerBones[j + num])->set_localEulerAngles(angles[j + num]);
	}
}