#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>

#include "main.hpp"

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/delegate.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Animator.hpp"
#include "UnityEngine/HumanBodyBones.hpp"
#include "UnityEngine/Keyframe.hpp"
#include "UnityEngine/AnimationCurve.hpp"

#include "customTypes/FinalIK/VRIK.hpp"
#include "customTypes/FinalIK/IKSolverVR.hpp"

#include "GlobalNamespace/OVRPlugin.hpp"
#include "GlobalNamespace/OVRPose.hpp"
#include "GlobalNamespace/SaberManager.hpp"

#include "utils/structs/OffsetPose.hpp"

namespace VRMQavatars
{
    class TargetManager : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars", TargetManager, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(TargetManager);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(TargetManager);
    public:
        DECLARE_INSTANCE_METHOD(void, Initialize);
        DECLARE_INSTANCE_METHOD(void, Update);
    public:
        UnityEngine::GameObject* noodleTrack;

        //fbt
        UnityEngine::GameObject* waistTracker;
        UnityEngine::GameObject* chestTracker;

        UnityEngine::GameObject* leftFootTracker;
        UnityEngine::GameObject* rightFootTracker;

        UnityEngine::GameObject* leftKneeTracker;
        UnityEngine::GameObject* rightKneeTracker;

        //basic
        UnityEngine::GameObject* leftHandTarget;
        UnityEngine::GameObject* rightHandTarget;

        UnityEngine::GameObject* headTarget;
        FinalIK::VRIK* vrik;
        bool intialized;

        UnityEngine::Vector3 replayLeftSaberPos;
        UnityEngine::Quaternion replayLeftSaberRot;
        UnityEngine::Vector3 replayRightSaberPos;
        UnityEngine::Quaternion replayRightSaberRot;

        Structs::OffsetPose offset;
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::TargetManager);