#include "AvatarManager.hpp"

#include <HandController.hpp>

#include "TPoseHelper.hpp"
#include "config/ConfigManager.hpp"
#include "customTypes/VRMWind.hpp"
#include "customTypes/BlendShape/BlendShapeController.hpp"

namespace VRMQavatars {
    AssetLib::Structure::VRM::VRMModelContext* AvatarManager::currentContext;
    RootMotion::FinalIK::VRIK* AvatarManager::vrik;
    TargetManager* AvatarManager::targetManager;
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

        if(context == nullptr)
        {
            OnLoad();
            return;
        }

        const auto root = currentContext->rootGameObject;

        vrik = root->GetComponent<RootMotion::FinalIK::VRIK*>();
        targetManager = root->GetComponent<TargetManager*>();
        _wristTwistFix = root->GetComponent<WristTwistFix*>();

        const auto blendShapeMaster = root->GetComponent<BlendShape::BlendShapeController*>();
        blendShapeMaster->Init();

        UpdateVRIK();

        SetHandOffset(Config::ConfigManager::GetOffsetSettings().handOffset);
        SetFingerPose(Config::ConfigManager::GetFingerPoseSettings().grabPose);

        TPoseHelper::SavePose(root->get_transform());

        const auto windSettings = Config::ConfigManager::GetWindSettings();
        const auto wind = root->AddComponent<VRMWind*>();
        wind->enableWind = windSettings.enabled;
        if(!windSettings.enabled)
        {
            wind->DisableWind();
        }
        wind->strengthFactor = windSettings.windStrength;
        wind->timeFactor = windSettings.timeFactor;

        OnLoad();
    }

    void AvatarManager::SetHandOffset(const Structs::OffsetPose& pose)
    {
        if(targetManager == nullptr) return;
        targetManager->offset = pose;
    }

    UnityEngine::Keyframe Frame(const float time, const float val)
    {
        UnityEngine::Keyframe frame;
        frame.m_Time = time;
        frame.m_Value = val;
        return frame;
    }

    ArrayW<UnityEngine::Keyframe> StepHeightFrames(const float val) {

        std::vector frames = {
            Frame(0.0f, 0.0f),
            Frame(0.5f, val),
            Frame(1.0f, 0.0f)
        };
        return il2cpp_utils::vectorToArray(frames);
    }

    void AvatarManager::UpdateVRIK()
    {
        if(vrik == nullptr || !vrik->solver->initiated) return;
        auto ikSettings = Config::ConfigManager::GetIKSettings();
        auto locoSettings = Config::ConfigManager::GetLocomotionSettings();
        vrik->solver->spine->bodyPosStiffness = ikSettings.bodyStiffness;
        vrik->solver->spine->headClampWeight = 0.38f;
        vrik->solver->spine->maintainPelvisPosition = 0.0f;
        vrik->solver->spine->minHeadHeight = -10.0f;

        vrik->solver->spine->maxRootAngle = 0.0f;

        vrik->solver->leftArm->swivelOffset = ikSettings.armSwivel;
        vrik->solver->rightArm->swivelOffset = ikSettings.armSwivel * -1.0f;
        vrik->solver->leftLeg->swivelOffset = ikSettings.legSwivel;
        vrik->solver->rightLeg->swivelOffset = ikSettings.legSwivel * -1.0f;

        vrik->solver->locomotion->footDistance = locoSettings.footDistance;
        vrik->solver->locomotion->stepThreshold = locoSettings.stepThreshold;
        vrik->solver->locomotion->angleThreshold = 45.0f;
        vrik->solver->locomotion->maxVelocity = 0.04f;
        vrik->solver->locomotion->velocityFactor = 0.04f;
        vrik->solver->locomotion->rootSpeed = 40.0f;
        vrik->solver->locomotion->stepSpeed = 2.0f;

        vrik->solver->locomotion->offset = locoSettings.stepOffset;

        vrik->solver->locomotion->stepHeight->set_keys(StepHeightFrames(locoSettings.stepHeight));
        vrik->solver->locomotion->heelHeight->set_keys(StepHeightFrames(locoSettings.stepHeight + 0.1f));
        vrik->UpdateSolverExternal();
    }

    void AvatarManager::SetFingerPose(const std::string& value)
    {
        if(vrik == nullptr) return;
        HandController::ApplyHandPose(vrik->animator, value);
    }
}
