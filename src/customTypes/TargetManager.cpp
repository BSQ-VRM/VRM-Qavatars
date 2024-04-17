#include "customTypes/TargetManager.hpp"

#include <bsml/shared/Helpers/utilities.hpp>

#include "customTypes/GroundOffsetObject.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"

#include "SceneEventManager.hpp"
#include "TPoseHelper.hpp"

#include "GlobalNamespace/OVRSkeletonRenderer.hpp"
#include "GlobalNamespace/OVRBody.hpp"
#include "GlobalNamespace/OVRPlugin.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Time.hpp"

#include "config/ConfigManager.hpp"

#include "conditional-dependencies/shared/main.hpp"

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
        leftHandTarget->transform->SetParent(transform, false);

        rightHandTarget = UnityEngine::GameObject::New_ctor();
        rightHandTarget->transform->SetParent(transform, false);

        headTarget = UnityEngine::GameObject::New_ctor();
        headTarget->transform->SetParent(transform, false);

        waistTracker = UnityEngine::GameObject::New_ctor();
        waistTracker->transform->SetParent(transform, false);

        chestTracker = UnityEngine::GameObject::New_ctor();
        chestTracker->transform->SetParent(transform, false);

        leftFootTracker = UnityEngine::GameObject::New_ctor();
        leftFootTracker->transform->SetParent(transform, false);

        rightFootTracker = UnityEngine::GameObject::New_ctor();
        rightFootTracker->transform->SetParent(transform, false);

        leftKneeTracker = UnityEngine::GameObject::New_ctor();
        leftKneeTracker->transform->SetParent(transform, false);

        rightKneeTracker = UnityEngine::GameObject::New_ctor();
        rightKneeTracker->transform->SetParent(transform, false);

        vrik->AutoDetectReferences();

        vrik->set_enabled(false);

        gameObject->AddComponent<GroundOffsetObject*>();
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
            auto camTrans = UnityEngine::GameObject::Find("PlayerTransformsHeadReplacement");
            if(camTrans == nullptr) return;
            headPos = camTrans->transform->position;
            headRot = camTrans->transform->rotation;

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
            if(name == "human://WAIST") transform = waistTracker->transform;
            if(name == "human://CHEST") transform = chestTracker->transform;
            if(name == "human://LEFT_FOOT") transform = leftFootTracker->transform;
            if(name == "human://RIGHT_FOOT") transform = rightFootTracker->transform;
            if(name == "human://LEFT_KNEE") transform = leftKneeTracker->transform;
            if(name == "human://RIGHT_KNEE") transform = rightKneeTracker->transform;
            if(transform != nullptr)
            {
                Sombrero::FastVector3 lastPos = transform->position;
                transform->set_position(Sombrero::FastVector3::Lerp(lastPos, tracker.pos, UnityEngine::Time::get_deltaTime() * 20.0f));
                transform->set_rotation(tracker.rot);
            }
        }

        if(SceneEventManager::inGame && noodleTrack)
        {
            auto ikConfig = Config::ConfigManager::GetIKSettings();

            auto pos = noodleTrack->transform->position;
            auto offsetPos = UnityEngine::Vector3::op_Addition(pos, UnityEngine::Vector3(0.0f, ikConfig.groundOffset, 0.0f));

            if(UnityEngine::Vector3::Distance(transform->position, offsetPos) > 0.05f)
            {
                transform->position = offsetPos;
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

        leftHandTarget->transform->position = leftHandPos;
        leftHandTarget->transform->rotation = leftHandRot;

        leftHandTarget->transform->Rotate({offset.rotX, offset.rotY, offset.rotZ});
        leftHandTarget->transform->Translate({offset.posX, offset.posY, offset.posZ});

        rightHandTarget->transform->position = rightHandPos;
        rightHandTarget->transform->rotation = rightHandRot;

        rightHandTarget->transform->Rotate({offset.rotX, offset.rotY, -offset.rotZ});
        rightHandTarget->transform->Translate({-offset.posX, offset.posY, offset.posZ});

        headTarget->transform->position = headPos;
        headTarget->transform->rotation = headRot;
    }
}