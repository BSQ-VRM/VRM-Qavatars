#pragma once

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

#include "RootMotion/FinalIK/VRIK.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Arm.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Spine.hpp"
#include "RootMotion/FinalIK/VRIK_References.hpp"
#include "RootMotion/FinalIK/IKSolver_UpdateDelegate.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Locomotion.hpp"

#include "GlobalNamespace/OVRPlugin.hpp"
#include "GlobalNamespace/OVRPlugin_Node.hpp"
#include "GlobalNamespace/OVRPlugin_Step.hpp"
#include "GlobalNamespace/OVRPose.hpp"
#include "GlobalNamespace/SaberManager.hpp"

#include "utils/structs/OffsetPose.hpp"

DECLARE_CLASS_CODEGEN(VRMQavatars, TargetManager, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, Initialize);
    DECLARE_INSTANCE_METHOD(void, Update);

    DECLARE_INSTANCE_METHOD(UnityEngine::Vector3, GetPosition, GlobalNamespace::OVRPlugin::Node node);
    DECLARE_INSTANCE_METHOD(float, GetAvatarHandHeight);
    DECLARE_INSTANCE_METHOD(float, GetAvatarHandDist);
    DECLARE_INSTANCE_METHOD(float, GetBase);

    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, vmcTracker);

    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, leftHandTarget);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, rightHandTarget);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, headTarget);
    DECLARE_INSTANCE_FIELD(RootMotion::FinalIK::VRIK*, vrik);
    DECLARE_INSTANCE_FIELD(bool, intialized);

    public:

    UnityEngine::Vector3 replayLeftSaberPos;
    UnityEngine::Quaternion replayLeftSaberRot;
    UnityEngine::Vector3 replayRightSaberPos;
    UnityEngine::Quaternion replayRightSaberRot;

    Structs::OffsetPose offset;

    void Calibrate(std::optional<float> scale = std::nullopt);

    float GetCalibrateScale(std::optional<float> baseScale = std::nullopt);
)