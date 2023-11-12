#pragma once

#include "main.hpp"

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Mathf.hpp"

#include "RootMotion/FinalIK/IKSolver_UpdateDelegate.hpp"
#include "RootMotion/FinalIK/VRIK.hpp"
#include "RootMotion/FinalIK/VRIK_References.hpp"

#include "custom-types/shared/delegate.hpp"

DECLARE_CLASS_CODEGEN(VRMQavatars, Mirror, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, SetVRIK, RootMotion::FinalIK::VRIK* vrik);
    DECLARE_INSTANCE_METHOD(void, OnPostUpdate);
    DECLARE_INSTANCE_METHOD(float, Dampen, float v, float min, float max);

    DECLARE_INSTANCE_FIELD_DEFAULT(float, elbowWeight, 0.5f);
    DECLARE_INSTANCE_FIELD_DEFAULT(float, shoulderWeight, 0.2f);
    DECLARE_INSTANCE_FIELD_DEFAULT(float, wristWeight, 0.4f);
    DECLARE_INSTANCE_FIELD(RootMotion::FinalIK::VRIK*, _vrik);
    DECLARE_INSTANCE_FIELD(RootMotion::FinalIK::IKSolver::UpdateDelegate*, postUpdateDelegate);
    public:
    FixData* _leftWristFixData;
    FixData* _rightWristFixData;
    FixData* _leftShoulderFixData;
    FixData* _rightShoulderFixData;
    FixData* _leftElbowFixData;
    FixData* _rightElbowFixData;
    void FixAxis(VRMQavatars::FixData* fixData, float weight, float dampen);
)