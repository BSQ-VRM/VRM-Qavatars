#include "CalibrationHelper.hpp"

#include <UnityEngine/Time.hpp>
#include <UnityEngine/WaitForEndOfFrame.hpp>

#include "GlobalNamespace/OVRPlugin.hpp"
#include "AvatarManager.hpp"
#include "HandController.hpp"
#include "TPoseHelper.hpp"
#include "config/ConfigManager.hpp"
#include "customTypes/VRMSpringBone.hpp"

namespace VRMQavatars
{
    void CalibrationHelper::PrepareContextForCalibration()
    {
        auto rootGameObject = AvatarManager::currentContext->rootGameObject->get_transform();

        for (const auto springBone : rootGameObject->GetComponentsInChildren<VRMSpringBone*>())
            springBone->set_enabled(false);

        const auto vrik = rootGameObject->GetComponent<FinalIK::VRIK*>();
        vrik->set_enabled(false);

        rootGameObject->set_position({0.0f, 0.0f, 0.0f});
        rootGameObject->set_eulerAngles({0.0f, 0.0f, 0.0f});
        rootGameObject->set_localScale({1.0f, 1.0f, 1.0f});

        vrik->solver->Reset();
        vrik->solver->FixTransforms();

        TPoseHelper::LoadPose();

        auto armScale = Config::ConfigManager::GetAvatarConfig().ArmCalibrationScale.GetValue() * (Config::ConfigManager::GetGlobalConfig().UncannyLongArmsFunnyUnderscoresReference.GetValue() ? 5.0f : 1.0f);
        auto legScale = Config::ConfigManager::GetAvatarConfig().LegCalibrationScale.GetValue();

        auto hips = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::Hips);
        auto LArm = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftUpperArm);
        auto RArm = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightUpperArm);
        auto LLeg = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftUpperLeg);
        auto RLeg = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightUpperLeg);

        LArm->set_localScale({armScale, LArm->localScale.y, LArm->localScale.z});
        RArm->set_localScale({armScale, RArm->localScale.y, RArm->localScale.z});
        LLeg->set_localScale({legScale, legScale, legScale});
        RLeg->set_localScale({legScale, legScale, legScale});

        auto localPos = hips->get_localPosition();
        hips->set_localPosition({localPos.x, localPos.y*legScale, localPos.z});
    }

    Sombrero::FastVector3 CalibrationHelper::GetPosition(const GlobalNamespace::OVRPlugin::Node node)
    {
        const auto pose = GlobalNamespace::OVRPlugin::GetNodePose(node, GlobalNamespace::OVRPlugin::Step::Render);
        const auto pos = Sombrero::FastVector3(pose.Position.x, pose.Position.y, -pose.Position.z);
        return pos;
    }

    float GetAvatarHandHeight()
    {
        const auto vrik = AvatarManager::vrik;
        auto avatarRightHandPos = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightHand)->get_position();
        auto avatarLeftHandPos = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftHand)->get_position();
        const float avatarHandAverageY = (avatarLeftHandPos.y + avatarRightHandPos.y) / 2.0f;
        return avatarHandAverageY;
    }

    float GetAvatarHandDist()
    {
        const auto vrik = AvatarManager::vrik;
        auto avatarRightHandPos = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightHand)->get_position();
        auto avatarLeftHandPos = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftHand)->get_position();
        const float avatarHandDist = UnityEngine::Vector3::Distance(avatarLeftHandPos, avatarRightHandPos);
        return avatarHandDist;
    }

    float GetBase()
    {
        const auto type = Config::ConfigManager::GetGlobalConfig().CalibrationType.GetValue();
        if(type == 0)
        {
            return GetAvatarHandDist();
        }
        if(type == 1)
        {
            return GetAvatarHandHeight();
        }
        return 1.0f;
    }

    float GetCalibrateScale(const std::optional<float> baseScale)
    {
        VRMLogger.info("scale: %d %f", baseScale.has_value(), baseScale.has_value() ? baseScale.value() : 0.0f);
        const auto type = Config::ConfigManager::GetGlobalConfig().CalibrationType.GetValue();
        if(type == 0)
        {
            const auto leftHandPos = CalibrationHelper::GetPosition(GlobalNamespace::OVRPlugin::Node::HandLeft);
            const auto rightHandPos = CalibrationHelper::GetPosition(GlobalNamespace::OVRPlugin::Node::HandRight);
            const float avatarHandDist = baseScale.has_value() ? baseScale.value() : GetAvatarHandDist();
            const float readDist = UnityEngine::Vector3::Distance(leftHandPos, rightHandPos);
            const float scale = readDist / avatarHandDist;
            VRMLogger.info("%f %f %f", readDist, avatarHandDist, scale);
            return scale * 0.85f;
        }
        if(type == 1)
        {
            const auto leftHandPos = CalibrationHelper::GetPosition(GlobalNamespace::OVRPlugin::Node::HandLeft);
            const auto rightHandPos = CalibrationHelper::GetPosition(GlobalNamespace::OVRPlugin::Node::HandRight);
            const float avatarHandAverageY = baseScale.has_value() ? baseScale.value() : GetAvatarHandHeight();
            const float readHandAverageY = (leftHandPos.y + rightHandPos.y) / 2.0f;
            const float scale = readHandAverageY / avatarHandAverageY;
            VRMLogger.info("%f %f %f", readHandAverageY, avatarHandAverageY, scale);
            return scale;
        }
        return Config::ConfigManager::GetGlobalConfig().FixedScale.GetValue();
    }

    void CalibrationHelper::Calibrate(const std::optional<float> scale)
    {
        auto rootGameObject = AvatarManager::currentContext->rootGameObject->get_transform();

        const auto vrik = AvatarManager::vrik;
        const auto targetManager = AvatarManager::targetManager;

        PrepareContextForCalibration();

        const float calibScale = scale.has_value() ? scale.value() : GetCalibrateScale(std::nullopt);
        VRMLogger.info("calibration scale %f", calibScale);

        auto& avaConfig = Config::ConfigManager::GetAvatarConfig();
        avaConfig.HasCalibrated.SetValue(true);
        avaConfig.CalibratedScale.SetValue(calibScale);

        rootGameObject->set_localScale(UnityEngine::Vector3(calibScale, calibScale, calibScale));

        HandController::ApplyHandPose(vrik->animator, Config::ConfigManager::GetFingerPoseSettings().grabPose);

        vrik->solver->spine->headTarget = targetManager->headTarget->get_transform();
        vrik->solver->leftArm->target = targetManager->leftHandTarget->get_transform();
        vrik->solver->rightArm->target = targetManager->rightHandTarget->get_transform();

        vrik->Initiate();
        targetManager->intialized = true;
        vrik->set_enabled(true);

        for (const auto springBone : rootGameObject->GetComponentsInChildren<VRMSpringBone*>())
            springBone->set_enabled(true);
    }

    custom_types::Helpers::Coroutine CalibrationHelper::StartCalibrationProc()
    {
        float time = 0.0f;
        auto rootGameObject = AvatarManager::currentContext->rootGameObject->get_transform();

        PrepareContextForCalibration();

        auto baseScale = GetBase();

        while(time < 4.0f)
        {
            time += UnityEngine::Time::get_deltaTime();

            auto headPos = GetPosition(GlobalNamespace::OVRPlugin::Node::Head);
            const auto leftHandPos = GetPosition(GlobalNamespace::OVRPlugin::Node::HandLeft);
            const auto rightHandPos = GetPosition(GlobalNamespace::OVRPlugin::Node::HandRight);

            headPos.y = 0.0f;

            const float yRotation = Sombrero::FastQuaternion::LookRotation(leftHandPos - rightHandPos, Sombrero::FastVector3::forward()).get_eulerAngles().y + 90.0f;

            rootGameObject->set_position(headPos);
            rootGameObject->set_rotation(UnityEngine::Quaternion::Euler(0.0f, yRotation, 0.0f));
            const float scale = GetCalibrateScale(baseScale);
            rootGameObject->set_localScale(Sombrero::FastVector3(scale, scale, scale));

            co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(UnityEngine::WaitForEndOfFrame::New_ctor()));
        }
        Calibrate(std::nullopt);
        co_return;
    }
}
