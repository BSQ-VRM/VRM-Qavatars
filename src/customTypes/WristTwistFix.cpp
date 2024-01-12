#include "customTypes/WristTwistFix.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp>
#include <sombrero/shared/FastQuaternion.hpp>
#include <sombrero/shared/FastVector3.hpp>

namespace VRMQavatars
{
    CP_SDK_IL2CPP_INHERIT_INIT(WristTwistFix);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(WristTwistFix)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(WristTwistFix)
    {

    }

    void WristTwistFix::SetVRIK(RootMotion::FinalIK::VRIK* vrik)
    {
        this->_vrik = vrik;

        if (vrik->references->leftUpperArm == nullptr|| vrik->references->leftForearm == nullptr|| vrik->references->leftHand == nullptr|| vrik->references->leftShoulder == nullptr|| vrik->references->rightUpperArm == nullptr|| vrik->references->rightForearm == nullptr|| vrik->references->rightHand == nullptr|| vrik->references->rightShoulder == nullptr)
        {
            _vrik = nullptr;
            _leftWristFixData = nullptr;
            _rightWristFixData = nullptr;
            _leftShoulderFixData = nullptr;
            _rightShoulderFixData = nullptr;
            _leftElbowFixData = nullptr;
            _rightElbowFixData = nullptr;
            return;
        }
        _leftWristFixData = new FixData(nullptr, vrik->references->leftForearm, vrik->references->leftHand);
        _rightWristFixData = new FixData(nullptr, vrik->references->rightForearm, vrik->references->rightHand);
        _leftShoulderFixData = new FixData(vrik->references->leftShoulder, vrik->references->leftUpperArm, vrik->references->leftForearm);
        _rightShoulderFixData = new FixData(vrik->references->rightShoulder, vrik->references->rightUpperArm, vrik->references->rightForearm);
        _leftElbowFixData = new FixData(nullptr, vrik->references->leftUpperArm, vrik->references->leftForearm);
        _rightElbowFixData = new FixData(nullptr, vrik->references->rightUpperArm, vrik->references->rightForearm);

        postUpdateDelegate = custom_types::MakeDelegate<RootMotion::FinalIK::IKSolver::UpdateDelegate*>(classof(RootMotion::FinalIK::IKSolver::UpdateDelegate*), static_cast<std::function<void()>>([this](){
            this->OnPostUpdate();
        }));

        vrik->GetIKSolver()->OnPostUpdate = reinterpret_cast<RootMotion::FinalIK::IKSolver::UpdateDelegate*>(RootMotion::FinalIK::IKSolver::UpdateDelegate::Combine(vrik->GetIKSolver()->OnPostUpdate, postUpdateDelegate));
    }


    float WristTwistFix::Dampen(float v, const float min, const float max)
    {
        const float num = (max - min) / 4.0f;
        const float num2 = min + num;
        const float num3 = min - num;
        const float num4 = max - num;
        const float num5 = max + num;
        if (v < num2)
        {
            if (v < num3)
            {
                v = num3;
            }
            const float num6 = (num2 - v) / (num2 - num3);
            return v + std::lerp(0.0f, min - num3, num6 * num6);
        }
        if (v > num4)
        {
            if (v > num5)
            {
                v = num5;
            }
            const float num7 = (v - num4) / (num5 - num4);
            return v - std::lerp(0.0f, num5 - max, num7 * num7);
        }
        return v;
    }

    void WristTwistFix::OnPostUpdate()
    {
        if (wristWeight > 0.0f)
        {
            FixAxis(_leftWristFixData, wristWeight, 90.0f);
            FixAxis(_rightWristFixData, wristWeight, 90.0f);
        }
        if (shoulderWeight > 0.0f)
        {
            FixAxis(_leftShoulderFixData, shoulderWeight, 180.0f);
            FixAxis(_rightShoulderFixData, shoulderWeight, 180.0f);
        }
        if (elbowWeight > 0.0f)
        {
            FixAxis(_leftElbowFixData, elbowWeight, 180.0f);
            FixAxis(_rightElbowFixData, elbowWeight, 180.0f);
        }
    }

    void WristTwistFix::FixAxis(FixData* fixData, const float weight, const float dampen)
    {
        if (fixData == nullptr)
        {
            return;
        }
        const Sombrero::FastQuaternion rotation = fixData->target->get_rotation();
        const Sombrero::FastVector3 vector =  Sombrero::FastQuaternion(fixData->target->get_rotation()) * fixData->axisRelativeToTargetDefault;
        const Sombrero::FastVector3 vector2 = Sombrero::FastQuaternion(fixData->child->get_rotation()) * fixData->axisRelativeToChildDefault;
        Sombrero::FastVector3 vector3;
        if (fixData->parent == nullptr)
        {
            vector3 = Sombrero::FastVector3::Slerp(vector, vector2, weight);
        }
        else
        {
            const Sombrero::FastVector3 vector4 = Sombrero::FastQuaternion(fixData->parent->get_rotation()) * fixData->axisRelativeToParentDefault;
            vector3 = Sombrero::FastVector3::Slerp(vector, vector4, weight);
        }
        vector3 = Sombrero::FastQuaternion(UnityEngine::Quaternion::Inverse(UnityEngine::Quaternion::LookRotation(rotation * fixData->axis, rotation * fixData->twistAxis))) * vector3;
        float num = std::atan2(vector3.x, vector3.z) * 57.29578f;
        num = UnityEngine::Mathf::Repeat(num + 180.0f, 360.0f) - 180.0f;
        num = Dampen(num, -dampen, dampen);
        const UnityEngine::Quaternion rotation2 = fixData->child->get_rotation();
        fixData->target->set_rotation(Sombrero::FastQuaternion(UnityEngine::Quaternion::AngleAxis(num, rotation * fixData->twistAxis)) * rotation);
        fixData->child->set_rotation(rotation2);
    }
}