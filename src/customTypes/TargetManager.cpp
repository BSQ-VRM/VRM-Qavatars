#include "customTypes/TargetManager.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp>
#include <UnityEngine/PrimitiveType.hpp>
#include <UnityEngine/Time.hpp>

#include "SceneEventManager.hpp"
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

        waistTracker = UnityEngine::GameObject::New_ctor();
        waistTracker->get_transform()->SetParent(get_transform(), false);

        chestTracker = UnityEngine::GameObject::New_ctor();
        chestTracker->get_transform()->SetParent(get_transform(), false);

        leftFootTracker = UnityEngine::GameObject::New_ctor();
        leftFootTracker->get_transform()->SetParent(get_transform(), false);

        rightFootTracker = UnityEngine::GameObject::New_ctor();
        rightFootTracker->get_transform()->SetParent(get_transform(), false);

        leftKneeTracker = UnityEngine::GameObject::New_ctor();
        leftKneeTracker->get_transform()->SetParent(get_transform(), false);

        rightKneeTracker = UnityEngine::GameObject::New_ctor();
        rightKneeTracker->get_transform()->SetParent(get_transform(), false);

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

        if(SceneEventManager::inGame && !noodleTrack)
        {
            noodleTrack = UnityEngine::GameObject::Find("NoodlePlayerTrack");
            GetComponent<GroundOffsetObject*>()->set_enabled(false);
        }

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

        auto trackers = VMC::VMCServer::availableTrackers;

        for(auto tracker : trackers)
        {
            auto name = tracker.name;
            UnityEngine::Transform* transform = nullptr;
            if(name == "human://WAIST") transform = waistTracker->get_transform();
            if(name == "human://CHEST") transform = chestTracker->get_transform();
            if(name == "human://LEFT_FOOT") transform = leftFootTracker->get_transform();
            if(name == "human://RIGHT_FOOT") transform = rightFootTracker->get_transform();
            if(name == "human://LEFT_KNEE") transform = leftKneeTracker->get_transform();
            if(name == "human://RIGHT_KNEE") transform = rightKneeTracker->get_transform();
            if(transform != nullptr)
            {
                Sombrero::FastVector3 lastPos = transform->get_position();
                transform->set_position(Sombrero::FastVector3::Lerp(lastPos, tracker.pos, UnityEngine::Time::get_deltaTime() * 20.0f));
                transform->set_rotation(tracker.rot);
            }
        }

        if(SceneEventManager::inGame && noodleTrack)
        {
            auto ikConfig = Config::ConfigManager::GetIKSettings();

            auto pos = noodleTrack->get_transform()->get_position();
            auto offsetPos = UnityEngine::Vector3::op_Addition(pos, UnityEngine::Vector3(0.0f, ikConfig.groundOffset, 0.0f));

            if(UnityEngine::Vector3::Distance(get_transform()->get_position(), offsetPos) > 0.05f)
            {
                get_transform()->set_position(offsetPos);
            }
            leftHandPos += pos;
            rightHandPos += pos;
            headPos += pos;

        }
        else
        {
            noodleTrack = nullptr;
            GetComponent<GroundOffsetObject*>()->set_enabled(true);
        }


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