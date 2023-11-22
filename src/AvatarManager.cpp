#include "AvatarManager.hpp"

#include <functional>
#include <HandController.hpp>

#include "config/ConfigManager.hpp"
#include "customTypes/BlendShape/BlendShapeController.hpp"

namespace VRMQavatars {
    AssetLib::Structure::VRM::VRMModelContext* AvatarManager::currentContext;
    RootMotion::FinalIK::VRIK* AvatarManager::_vrik;
    TargetManager* AvatarManager::_targetManager;
    WristTwistFix* AvatarManager::_wristTwistFix;
    CP_SDK::Utils::Event<> AvatarManager::OnLoad;

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

        auto blendShapeMaster = root->GetComponent<BlendShape::BlendShapeController*>();
        blendShapeMaster->Init();

        UpdateVRIK();

        SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
        SetFingerPose(Config::ConfigManager::GetFingerPosingSettings().grabPose);

        OnLoad();
    }

    ArrayW<UnityEngine::Keyframe> GetStepFrames(const float val) {
        auto array = ArrayW<UnityEngine::Keyframe>(3);
        array[0].m_Time = 0.0f;
        array[0].m_Value = 0.0f;
        array[1].m_Time = 0.5f;
        array[1].m_Value = val;
        array[2].m_Time = 1.0f;
        array[2].m_Value = 0.0f;
        return array;
    }

    void AvatarManager::UpdateVRIK()
    {
        if(_vrik == nullptr || !_vrik->solver->initiated) return;
        auto ikSettings = Config::ConfigManager::GetIKSettings();
        auto locoSettings = Config::ConfigManager::GetLocomotionSettings();
        _vrik->solver->spine->bodyPosStiffness = ikSettings.bodyStiffness;
        _vrik->solver->spine->headClampWeight = 0.38f;
        _vrik->solver->spine->maintainPelvisPosition = 0.0f;
        _vrik->solver->spine->minHeadHeight = -10.0f;

        _vrik->solver->spine->maxRootAngle = 0.0f;

        _vrik->solver->leftArm->swivelOffset = ikSettings.armSwivel;
        _vrik->solver->rightArm->swivelOffset = ikSettings.armSwivel * -1.0f;
        _vrik->solver->leftLeg->swivelOffset = ikSettings.legSwivel;
        _vrik->solver->rightLeg->swivelOffset = ikSettings.legSwivel * -1.0f;

        _vrik->solver->locomotion->footDistance = locoSettings.footDistance;
        _vrik->solver->locomotion->stepThreshold = locoSettings.stepThreshold;
        _vrik->solver->locomotion->angleThreshold = 45.0f;
        _vrik->solver->locomotion->maxVelocity = 0.04f;
        _vrik->solver->locomotion->velocityFactor = 0.04f;
        _vrik->solver->locomotion->rootSpeed = 40.0f;
        _vrik->solver->locomotion->stepSpeed = 2.0f;

        _vrik->solver->locomotion->offset = locoSettings.stepOffset;

        _vrik->solver->locomotion->stepHeight->set_keys(GetStepFrames(locoSettings.stepHeight));
        _vrik->solver->locomotion->heelHeight->set_keys(GetStepFrames(locoSettings.stepHeight + 0.05f));
        _vrik->UpdateSolverExternal();
    }

    void AvatarManager::SetFingerPose(std::string value)
    {
        if(_vrik == nullptr) return;
        HandController::ApplyHandPose(_vrik->animator, value);
    }
}
