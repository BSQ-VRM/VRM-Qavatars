#include "customTypes/TargetManager.hpp"

#include <UnityEngine/PrimitiveType.hpp>

#include "customTypes/WristTwistFix.hpp"
#include "HandController.hpp"
#include "TPoseHelper.hpp"

#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Resources.hpp"

#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/OVRPlugin.hpp"

#include "RootMotion/FinalIK/Finger.hpp"
#include "RootMotion/FinalIK/FingerRig.hpp"

#include "config/ConfigManager.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "customTypes/GroundOffsetObject.hpp"
#include "VMC/VMCClient.hpp"
#include "VMC/VMCServer.hpp"

DEFINE_TYPE(VRMQavatars, TargetManager);

void VRMQavatars::TargetManager::Initialize()
{
    leftHandTarget = UnityEngine::GameObject::New_ctor();
    leftHandTarget->get_transform()->SetParent(get_transform(), false);

    rightHandTarget = UnityEngine::GameObject::New_ctor();
    rightHandTarget->get_transform()->SetParent(get_transform(), false);

    headTarget = UnityEngine::GameObject::New_ctor();
    headTarget->get_transform()->SetParent(get_transform(), false);

    vrik->AutoDetectReferences();

    vrik->set_enabled(false);

    get_gameObject()->AddComponent<GroundOffsetObject*>();
}
void VRMQavatars::TargetManager::Update()
{
    if(!intialized)
        return;

    Sombrero::FastVector3 leftHandPos;
    UnityEngine::Quaternion leftHandRot;

    Sombrero::FastVector3 rightHandPos;
    UnityEngine::Quaternion rightHandRot;

    Sombrero::FastVector3 headPos;
    UnityEngine::Quaternion headRot;

    const static auto replay = CondDeps::Find<bool>("replay", "IsInReplay");
    if(replay.has_value() && replay.value()())
    {
        const auto camTrans = UnityEngine::GameObject::Find("PlayerTransformsHeadReplacement");
        if(camTrans == nullptr) return;
        headPos = camTrans->get_transform()->get_position();
        headRot = camTrans->get_transform()->get_rotation();

        leftHandPos = replayLeftSaberPos;
        leftHandRot = replayLeftSaberRot;

        rightHandPos = replayRightSaberPos;
        rightHandRot = replayRightSaberRot;
    }
    else
    {
        const auto leftHandPose = GlobalNamespace::OVRPlugin::GetNodePose(GlobalNamespace::OVRPlugin::Node::HandLeft, GlobalNamespace::OVRPlugin::Step::Render);
        leftHandPos = {leftHandPose.Position.x, leftHandPose.Position.y, -leftHandPose.Position.z};
        leftHandRot = {-leftHandPose.Orientation.x, -leftHandPose.Orientation.y, leftHandPose.Orientation.z, leftHandPose.Orientation.w};

        const auto rightHandPose = GlobalNamespace::OVRPlugin::GetNodePose(GlobalNamespace::OVRPlugin::Node::HandRight, GlobalNamespace::OVRPlugin::Step::Render);
        rightHandPos = {rightHandPose.Position.x, rightHandPose.Position.y, -rightHandPose.Position.z};
        rightHandRot = {-rightHandPose.Orientation.x, -rightHandPose.Orientation.y, rightHandPose.Orientation.z, rightHandPose.Orientation.w};

        const auto headPose = GlobalNamespace::OVRPlugin::GetNodePose(GlobalNamespace::OVRPlugin::Node::Head, GlobalNamespace::OVRPlugin::Step::Render);
        headPos = {headPose.Position.x, headPose.Position.y, -headPose.Position.z};
        headRot = {-headPose.Orientation.x, -headPose.Orientation.y, headPose.Orientation.z, headPose.Orientation.w};
    }

    VMC::VMCClient::SendHeadsetPos(headPos, headRot);
    VMC::VMCClient::SendControllerPos(leftHandPos, leftHandRot, 0);
    VMC::VMCClient::SendControllerPos(rightHandPos, rightHandRot, 1);

    VMC::VMCServer::Receive();

    leftHandTarget->get_transform()->set_position(leftHandPos);
    leftHandTarget->get_transform()->set_rotation(leftHandRot);

    leftHandTarget->get_transform()->Rotate({offset.rotX, offset.rotY, offset.rotZ});
    leftHandTarget->get_transform()->Translate({offset.posX, offset.posY, offset.posZ});

    rightHandTarget->get_transform()->set_position(rightHandPos);
    rightHandTarget->get_transform()->set_rotation(rightHandRot);

    rightHandTarget->get_transform()->Rotate({offset.rotX, offset.rotY, -offset.rotZ});
    rightHandTarget->get_transform()->Translate({-offset.posX, offset.posY, offset.posZ});

    headTarget->get_transform()->set_position(headPos);
    headTarget->get_transform()->set_rotation(headRot);
}

void VRMQavatars::TargetManager::Calibrate(const std::optional<float> scale)
{
    vrik->set_enabled(false);

    get_transform()->set_position({0.0f, 0.0f, 0.0f});
    get_transform()->set_eulerAngles({0.0f, 0.0f, 0.0f});
    get_transform()->set_localScale({1.0f, 1.0f, 1.0f});

    TPoseHelper::LoadPose();

    auto armScale = Config::ConfigManager::GetAvatarConfig().ArmCalibrationScale.GetValue();
    auto legScale = Config::ConfigManager::GetAvatarConfig().LegCalibrationScale.GetValue();

    const auto hips = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::Hips);
    const auto LArm = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftUpperArm);
    const auto RArm = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightUpperArm);
    const auto LLeg = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftUpperLeg);
    const auto RLeg = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightUpperLeg);

    LArm->set_localScale({armScale, armScale, armScale});
    RArm->set_localScale({armScale, armScale, armScale});

    LLeg->set_localScale({legScale, legScale, legScale});
    RLeg->set_localScale({legScale, legScale, legScale});

    auto localPos = hips->get_localPosition();
    hips->set_localPosition({localPos.x, localPos.y*legScale, localPos.z});

    const float calibScale = scale.has_value() ? scale.value() : GetCalibrateScale();

    auto& avaConfig = Config::ConfigManager::GetAvatarConfig();
    avaConfig.HasCalibrated.SetValue(true);
    avaConfig.CalibratedScale.SetValue(calibScale);

    get_transform()->set_localScale(UnityEngine::Vector3(calibScale, calibScale, calibScale));

    HandController::ApplyHandPose(vrik->animator, Config::ConfigManager::GetFingerPosingSettings().grabPose);

    vrik->solver->spine->headTarget = headTarget->get_transform();
    vrik->solver->leftArm->target = leftHandTarget->get_transform();
    vrik->solver->rightArm->target = rightHandTarget->get_transform();
    vrik->Initiate();

    intialized = true;
    vrik->set_enabled(true);

    get_gameObject()->AddComponent<WristTwistFix*>()->SetVRIK(vrik);
}

UnityEngine::Vector3 VRMQavatars::TargetManager::GetPosition(const GlobalNamespace::OVRPlugin::Node node)
{
    const auto pose = GlobalNamespace::OVRPlugin::GetNodePose(node, GlobalNamespace::OVRPlugin::Step::Render);
    const auto pos = UnityEngine::Vector3(pose.Position.x, pose.Position.y, -pose.Position.z);
    return pos;
}

float VRMQavatars::TargetManager::GetAvatarHandHeight()
{
    const auto avatarRightHandPos = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightHand)->get_position();
    const auto avatarLeftHandPos = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftHand)->get_position();

    const float avatarHandAverageY = (avatarLeftHandPos.y + avatarRightHandPos.y) / 2.0f;

    return avatarHandAverageY;
}

float VRMQavatars::TargetManager::GetAvatarHandDist()
{
    const auto avatarRightHandPos = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightHand)->get_position();
    const auto avatarLeftHandPos = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftHand)->get_position();

    const float avatarHandDist = UnityEngine::Vector3::Distance(avatarLeftHandPos, avatarRightHandPos);

    return avatarHandDist;
}

float VRMQavatars::TargetManager::GetBase()
{
    const auto type = Config::ConfigManager::GetGlobalConfig().CalibrationType.GetValue();
    if(type == "Match Armspans")
    {
        return GetAvatarHandDist();
    }
    if(type == "Match Heights")
    {
        return GetAvatarHandHeight();
    }
    return 1.0f;
}


float VRMQavatars::TargetManager::GetCalibrateScale(const std::optional<float> baseScale)
{
    const auto type = Config::ConfigManager::GetGlobalConfig().CalibrationType.GetValue();
    if(type == "Match Armspans")
    {
        const auto leftHandPos = GetPosition(GlobalNamespace::OVRPlugin::Node::HandLeft);
        const auto rightHandPos = GetPosition(GlobalNamespace::OVRPlugin::Node::HandRight);

        const float avatarHandDist = baseScale.has_value() ? baseScale.value() : GetAvatarHandDist();

        const float readDist = UnityEngine::Vector3::Distance(leftHandPos, rightHandPos);
        const float scale = readDist / avatarHandDist;

        getLogger().info("%f %f %f", readDist, avatarHandDist, scale);

        return scale * 0.85f;
    }
    if(type == "Match Heights")
    {
        const auto leftHandPos = GetPosition(GlobalNamespace::OVRPlugin::Node::HandLeft);
        const auto rightHandPos = GetPosition(GlobalNamespace::OVRPlugin::Node::HandRight);

        const float avatarHandAverageY = baseScale.has_value() ? baseScale.value() : GetAvatarHandHeight();

        const float readHandAverageY = (leftHandPos.y + rightHandPos.y) / 2.0f;
        const float scale = readHandAverageY / avatarHandAverageY;

        getLogger().info("%f %f %f", readHandAverageY, avatarHandAverageY, scale);

        return scale;
    }

    return Config::ConfigManager::GetGlobalConfig().FixedScale.GetValue();
}