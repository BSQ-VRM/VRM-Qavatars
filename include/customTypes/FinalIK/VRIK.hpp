#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>

#include "IKSolverVR.hpp"
#include "References.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Animator.hpp"
#include "UnityEngine/Animation.hpp"

namespace VRMQavatars::FinalIK
{
    class VRIK : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars", VRIK, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(VRIK);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(VRIK);
        
        DECLARE_INSTANCE_METHOD(void, OnDisable);
        DECLARE_INSTANCE_METHOD(void, Start);
        DECLARE_INSTANCE_METHOD(void, Update);
        DECLARE_INSTANCE_METHOD(void, LateUpdate);
        DECLARE_INSTANCE_METHOD(void, FixedUpdate);
    public:
        //Solver Manager
        void Disable();
        void FindAnimatorRecursive(UnityEngine::Transform* t, bool findInChildren);
        void Initiate();
        bool animatePhysics();
        bool isAnimated();

        bool fixTransforms = true;
        UnityEngine::Animator* animator;
        UnityEngine::Animation* legacy;
        bool updateFrame;
        bool componentInitiated;
        bool skipSolverUpdate;

        //IK
        void IK_UpdateSolver();
        void IK_InitiateSolver();

        //VRIK
        void AutoDetectReferences();
        IKSolverVR* GetIKSolver() const;
        void VRIK_InitiateSolver();
        void VRIK_UpdateSolver();
        void UpdateSolverExternal();
        void FixTransforms() const;

        References* references = new References();;
        IKSolverVR* solver = new IKSolverVR();
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::FinalIK::VRIK);