#include "customTypes/TargetManager.hpp"

#include "HandController.hpp"
#include <customTypes/WristTwistFix.hpp>

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
}
void VRMQavatars::TargetManager::Update()
{
    if(!intialized)
        return;
    auto leftHandPose = GlobalNamespace::OVRPlugin::GetNodePose(GlobalNamespace::OVRPlugin::Node::HandLeft, GlobalNamespace::OVRPlugin::Step::Render);
	auto leftHandPos = UnityEngine::Vector3(leftHandPose.Position.x, leftHandPose.Position.y, -leftHandPose.Position.z);
	auto leftHandRot = UnityEngine::Quaternion(-leftHandPose.Orientation.x, -leftHandPose.Orientation.y, leftHandPose.Orientation.z, leftHandPose.Orientation.w);

    auto rightHandPose = GlobalNamespace::OVRPlugin::GetNodePose(GlobalNamespace::OVRPlugin::Node::HandRight, GlobalNamespace::OVRPlugin::Step::Render);
	auto rightHandPos = UnityEngine::Vector3(rightHandPose.Position.x, rightHandPose.Position.y, -rightHandPose.Position.z);
	auto rightHandRot = UnityEngine::Quaternion(-rightHandPose.Orientation.x, -rightHandPose.Orientation.y, rightHandPose.Orientation.z, rightHandPose.Orientation.w);

    auto headPose = GlobalNamespace::OVRPlugin::GetNodePose(GlobalNamespace::OVRPlugin::Node::Head, GlobalNamespace::OVRPlugin::Step::Render);
	auto headPos = UnityEngine::Vector3(headPose.Position.x, headPose.Position.y, -headPose.Position.z);
	auto headRot = UnityEngine::Quaternion(-headPose.Orientation.x, -headPose.Orientation.y, headPose.Orientation.z, headPose.Orientation.w);

    leftHandTarget->get_transform()->set_position(leftHandPos);
    leftHandTarget->get_transform()->set_rotation(leftHandRot);
    
    leftHandTarget->get_transform()->Rotate(UnityEngine::Vector3(offset.rotX, offset.rotY, offset.rotZ));
    leftHandTarget->get_transform()->Translate(UnityEngine::Vector3(offset.posX, offset.posY, offset.posZ));

    rightHandTarget->get_transform()->set_position(rightHandPos);
    rightHandTarget->get_transform()->set_rotation(rightHandRot);

    rightHandTarget->get_transform()->Rotate(UnityEngine::Vector3(offset.rotX, offset.rotY, -offset.rotZ));
    rightHandTarget->get_transform()->Translate(UnityEngine::Vector3(-offset.posX, offset.posY, offset.posZ));

    headTarget->get_transform()->set_position(headPos);
    headTarget->get_transform()->set_rotation(headRot);
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

void VRMQavatars::TargetManager::Calibrate()
{
    get_transform()->set_position(UnityEngine::Vector3(0.0f, 0.0f, 0.0f));
    get_transform()->set_eulerAngles(UnityEngine::Vector3(0.0f, 0.0f, 0.0f));
    get_transform()->set_localScale(UnityEngine::Vector3(1.0f, 1.0f, 1.0f));
	float scale = GetCalibrateScale();

    get_transform()->set_localScale(UnityEngine::Vector3(scale, scale, scale));

    HandController::ApplyHandPose(vrik->animator, "-34,-66,-80,0,-34,-66,-65,0,-37,-73,-43,0,-46,-48,-30,0,-77,-47,-20,19");

    vrik->solver->spine->headTarget = headTarget->get_transform();
    vrik->solver->leftArm->target = leftHandTarget->get_transform();
    vrik->solver->rightArm->target = rightHandTarget->get_transform();
    vrik->Initiate();

    intialized = true;
    vrik->set_enabled(true);

    get_gameObject()->AddComponent<WristTwistFix*>()->SetVRIK(vrik);

    vrik->solver->locomotion->footDistance = 0.1f;
    vrik->solver->locomotion->stepThreshold = 0.1f;
    vrik->solver->locomotion->stepHeight->set_keys(GetStepFrames(0.02f));
    vrik->solver->locomotion->heelHeight->set_keys(GetStepFrames(0.1f));
}

UnityEngine::Vector3 VRMQavatars::TargetManager::GetPosition(GlobalNamespace::OVRPlugin::Node node)
{
    auto pose = GlobalNamespace::OVRPlugin::GetNodePose(node, GlobalNamespace::OVRPlugin::Step::Render);
	auto pos = UnityEngine::Vector3(pose.Position.x, pose.Position.y, -pose.Position.z);
    return pos;
}

float VRMQavatars::TargetManager::GetCalibrateScale()
{
    auto leftHandPos = GetPosition(GlobalNamespace::OVRPlugin::Node::HandLeft);
	auto rightHandPos = GetPosition(GlobalNamespace::OVRPlugin::Node::HandRight);

    auto avatarRightHandPos = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::RightHand)->get_position();
    auto avatarLeftHandPos = vrik->animator->GetBoneTransform(UnityEngine::HumanBodyBones::LeftHand)->get_position();

    float readHandAverageY = (leftHandPos.y + rightHandPos.y) / 2.0f;
	float avatarHandAverageY = (avatarLeftHandPos.y + avatarRightHandPos.y) / 2.0f;
	float scale = readHandAverageY / avatarHandAverageY;

    getLogger().info("%f %f %f", readHandAverageY, avatarHandAverageY, scale);

    return scale;
}