#include "customTypes/TargetManager.hpp"

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
    vrik->solver->spine->headTarget = headTarget->get_transform();
    vrik->solver->leftArm->target = leftHandTarget->get_transform();
    vrik->solver->rightArm->target = rightHandTarget->get_transform();
    vrik->Initiate();

    intialized = true;
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

    headPos.z += 2.0f;
    leftHandPos.z += 2.0f;
    rightHandPos.z += 2.0f;

    leftHandTarget->get_transform()->set_position(leftHandPos);
    leftHandTarget->get_transform()->set_rotation(leftHandRot);

    rightHandTarget->get_transform()->set_position(rightHandPos);
    rightHandTarget->get_transform()->set_rotation(rightHandRot);

    headTarget->get_transform()->set_position(headPos);
    headTarget->get_transform()->set_rotation(headRot);
}