#include "HandController.hpp"
#include <sstream>
#include <sombrero/shared/FastVector3.hpp>

#include "UnityEngine/HumanBodyBones.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Vector3.hpp"

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

namespace VRMQavatars
{
	std::vector<std::string> HandController::SplitPose(const std::string& pose)
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

	std::vector<float> HandController::ParseRotations(const std::vector<std::string>& pose)
	{
		std::vector<float> ret = std::vector<float>(pose.size());
		for (size_t i = 0; i < pose.size(); i++)
		{
			ret[i] = static_cast<float>(stoi(pose[i]));
		}

		return ret;
	}

	int HandController::GetValue(const std::string& pose, const int idx)
	{
		const auto values = ParseRotations(SplitPose(pose));
		if(values.size()-1 < idx) return 0;
		return values[idx];
	}

	std::string HandController::SetValue(const std::string& pose, const int idx, const int value)
	{
		auto values = ParseRotations(SplitPose(pose));
		values[idx] = value;

		auto newValues = std::vector<std::string>(values.size());
		for (int i = 0; i < values.size(); ++i)
		{
			newValues[i] = std::to_string(values[i]);
		}

		const char* const delim = ",";

		std::ostringstream imploded;
		std::copy(newValues.begin(), newValues.end(),
				   std::ostream_iterator<std::string>(imploded, delim));

		return imploded.str();
	}

	void HandController::ApplyHandPose(UnityEngine::Animator* animator, std::string pose)
	{
		auto array = SplitPose(std::move(pose));
		auto rotations = ParseRotations(array);

		auto angles = std::vector<Sombrero::FastVector3>(fingerBones.size());
		auto boneAngles = std::vector<Sombrero::FastVector3>(fingerBones.size());
		for (int i = 0; i < fingerBones.size(); i++)
		{
			boneAngles[i] = UnityEngine::Quaternion::get_identity().get_eulerAngles();
		}

		if (rotations.size() != 20)
		{
			return;
		}

		int num = fingerBones.size() / 2;

		constexpr int jointsPerDigit = 3;

		for (int i = 0; i < num; i += jointsPerDigit)
		{
			//First Four Fingers
			if (i >= 12)
			{
				//Bone 1
				auto bone1Angle = boneAngles[i + 2];
				float rot1 = rotations[i / 3 * 4 + 3];
				float rot2 = rotations[i / 3 * 4 + 2];
				angles[i + 2] = Sombrero::FastVector3(bone1Angle.x, bone1Angle.y - rot1, bone1Angle.z - rot2);

				//Bone 2
				auto bone2Angle = boneAngles[i + 1];
				float bone2Rot = -rotations[i / 3 * 4 + 1] / 90.0f;
				angles[i + 1] = Sombrero::FastVector3(bone2Angle.x, bone2Angle.y - bone2Rot * 38.0f, bone2Angle.z - bone2Rot * -15.0f);

				//Bone 3
				auto bone3Angle = boneAngles[i];
				float bone3Rot = -rotations[i / 3 * 4] / 90.0f;
				angles[i] = Sombrero::FastVector3(bone3Angle.x + bone3Rot * 34.0f, bone3Angle.y - bone3Rot * 56.0f, bone3Angle.z - bone3Rot * -7.0f);
			}
			else //Thumb
			{
				//Bone 1
				auto bone1Angle = boneAngles[i + 2];
				float rot1 = rotations[i / 3 * 4 + 2];
				float rot2 = rotations[i / 3 * 4 + 3];
				angles[i + 2] = Sombrero::FastVector3(bone1Angle.x, bone1Angle.y - rot2, bone1Angle.z - rot1);

				//Bone 2
				auto bone2Angle = boneAngles[i + 1];
				auto bone1Rot = rotations[i / 3 * 4 + 1];
				angles[i + 1] = Sombrero::FastVector3(bone2Angle.x, bone2Angle.y, bone2Angle.z - bone1Rot);

				//Bone 3
				auto bone3Angle = boneAngles[i];
				auto bone3Rot = rotations[i / 3 * 4];
				angles[i] = Sombrero::FastVector3(bone3Angle.x, bone3Angle.y, bone3Angle.z - bone3Rot);
			}
		}
		for (int j = 0; j < num; j += jointsPerDigit)
		{
			//First Four Fingers
			if (j >= 12)
			{
				//Bone 1
				auto vector3 = boneAngles[j + num + 2];
				float num14 = rotations[j / 3 * 4 + 3];
				float num15 = rotations[j / 3 * 4 + 2];
				angles[j + num + 2] = Sombrero::FastVector3(vector3.x, vector3.y + num14, vector3.z + num15);

				//Bone 2
				vector3 = boneAngles[j + num + 1];
				float num16 = -rotations[j / 3 * 4 + 1] / 90.0f;
				angles[j + num + 1] = Sombrero::FastVector3(vector3.x + num16 * 38.0f, vector3.y + num16 * 38.0f, vector3.z + num16 * -15.0f);

				//Bone 3
				vector3 = boneAngles[j + num];
				float num17 = -rotations[j / 3 * 4] / 90.0f;
				angles[j + num] = Sombrero::FastVector3(vector3.x + num17 * 34.0f, vector3.y + num17 * 56.0f, vector3.z + num17 * -7.0f);
			}
			else //Thumb
			{
				//Bone 1
				auto bone1Angle = boneAngles[j + num + 2];
				float rot1 = rotations[j / 3 * 4 + 2];
				float rot2 = rotations[j / 3 * 4 + 3];
				angles[j + num + 2] = Sombrero::FastVector3(bone1Angle.x, bone1Angle.y + rot2, bone1Angle.z + rot1);

				//Bone 2
				auto bone2Angle = boneAngles[j + num + 1];
				auto bone2Rot = rotations[j / 3 * 4 + 1];
				angles[j + num + 1] = Sombrero::FastVector3(bone2Angle.x, bone2Angle.y, bone2Angle.z + bone2Rot);

				//Bone 3
				auto bone3Angle = boneAngles[j + num];
				auto bone3Rot = rotations[j / 3 * 4];
				angles[j + num] = Sombrero::FastVector3(bone3Angle.x, bone3Angle.y, bone3Angle.z + bone3Rot);
			}
		}

		for (int j = 0; j < num; j++)
		{
			auto trans = animator->GetBoneTransform(fingerBones[j]);
			if(trans != nullptr)
			{
				trans->set_localEulerAngles(angles[j]);
			}
		}

		for (int j = 0; j < num; j++)
		{
			auto trans = animator->GetBoneTransform(fingerBones[j + num]);
			if(trans != nullptr)
			{
				trans->set_localEulerAngles(angles[j + num]);
			}
		}
	}
}