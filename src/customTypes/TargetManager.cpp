#include "customTypes/TargetManager.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp>
#include <UnityEngine/PrimitiveType.hpp>

#include "customTypes/WristTwistFix.hpp"

#include "UnityEngine/Camera.hpp"

#include "GlobalNamespace/OVRPlugin.hpp"

#include "config/ConfigManager.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "customTypes/GroundOffsetObject.hpp"
#include "VMC/VMCClient.hpp"
#include "VMC/VMCServer.hpp"

#include "sombrero/shared/FastQuaternion.hpp"

namespace VRMQavatars
{
    CP_SDK_IL2CPP_INHERIT_INIT(TargetManager);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(TargetManager)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(TargetManager)
    {

    }

    void TargetManager::Initialize()
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

    void TargetManager::Update()
    {
        if(!intialized)
            return;

        Sombrero::FastVector3 leftHandPos;
        Sombrero::FastQuaternion leftHandRot;

        Sombrero::FastVector3 rightHandPos;
        Sombrero::FastQuaternion rightHandRot;

        Sombrero::FastVector3 headPos;
        Sombrero::FastQuaternion headRot;

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
}