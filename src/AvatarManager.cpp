#include "AvatarManager.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/Unity/Extensions/ColorU.hpp>

namespace VRMQavatars {
    AssetLib::Structure::VRM::VRMModelContext* AvatarManager::currentContext;
    RootMotion::FinalIK::VRIK* AvatarManager::_vrik;
    TargetManager* AvatarManager::_targetManager;

    void AvatarManager::SetContext(AssetLib::Structure::VRM::VRMModelContext* context)
    {
        if(currentContext != nullptr)
        {
            UnityEngine::GameObject::Destroy(currentContext->rootGameObject);
            delete currentContext;
        }
        currentContext = context;
        const auto root = currentContext->rootGameObject;
        _vrik = root->GetComponent<RootMotion::FinalIK::VRIK*>();
        _targetManager = root->GetComponent<TargetManager*>();
        _wristTwistFix = root->GetComponent<WristTwistFix*>();
    }

    void AvatarManager::SetLegSwivel(const float value)
    {
        _vrik->solver->leftLeg->swivelOffset = value;
        _vrik->solver->rightLeg->swivelOffset = value;
    }

    void AvatarManager::SetArmSwivel(const float value)
    {
        _vrik->solver->leftArm->swivelOffset = value;
        _vrik->solver->rightArm->swivelOffset = value;
    }

    void AvatarManager::SetHandOffset(const Structs::OffsetPose& pose)
    {
        _targetManager->offset = pose;
    }

    void AvatarManager::SetBodyStiffness(float value)
    {
        _vrik->solver->spine->bodyPosStiffness = value;
    }

    void AvatarManager::SetShoulderRotation(float value)
    {
        _vrik->solver->leftArm->shoulderRotationWeight = value;
        _vrik->solver->rightArm->shoulderRotationWeight = value;
    }

    void AvatarManager::SetShoulderFixWeight(float value)
    {
        _wristTwistFix->shoulderWeight = value;
    }

    void AvatarManager::SetWristFixWeight(float value)
    {
        _wristTwistFix->wristWeight = value;
    }
}
