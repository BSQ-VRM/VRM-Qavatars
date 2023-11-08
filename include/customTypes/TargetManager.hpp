#pragma once

#include "main.hpp"

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"

#include "RootMotion/FinalIK/VRIK.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Arm.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Spine.hpp"
#include "RootMotion/FinalIK/VRIK_References.hpp"

#include "GlobalNamespace/OVRPlugin.hpp"
#include "GlobalNamespace/OVRPlugin_Node.hpp"
#include "GlobalNamespace/OVRPlugin_Step.hpp"
#include "GlobalNamespace/OVRPose.hpp"

DECLARE_CLASS_CODEGEN(VRMQavatars, TargetManager, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, Initialize);
    DECLARE_INSTANCE_METHOD(void, Update);

    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, leftHandTarget);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, rightHandTarget);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, headTarget);
    DECLARE_INSTANCE_FIELD(RootMotion::FinalIK::VRIK*, vrik);
    DECLARE_INSTANCE_FIELD(bool, intialized);
)