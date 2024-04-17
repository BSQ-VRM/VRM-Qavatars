#include "customTypes/FinalIK/VRIK.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp>

#include "UnityEngine/AnimatorUpdateMode.hpp"
#include "main.hpp"

namespace VRMQavatars::FinalIK
{
    CP_SDK_IL2CPP_INHERIT_INIT(VRIK);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(VRIK)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(VRIK)
    {

    }

    //Solver Manager
    void VRIK::Disable() {
        enabled = false;
    }

    void VRIK::FindAnimatorRecursive(UnityEngine::Transform* t, bool findInChildren) {
        if (isAnimated())
        {
            return;
        }
        animator = t->GetComponent<UnityEngine::Animator*>();
        legacy = t->GetComponent<UnityEngine::Animation*>();
        if (isAnimated())
        {
            return;
        }
        if (animator == nullptr && findInChildren)
        {
            animator = t->GetComponentInChildren<UnityEngine::Animator*>();
        }
        if (legacy == nullptr && findInChildren)
        {
            legacy = t->GetComponentInChildren<UnityEngine::Animation*>();
        }
        if (!isAnimated() && t->parent != nullptr)
        {
            FindAnimatorRecursive(t->parent, false);
        }
    }

    void VRIK::FixedUpdate() {
        if (skipSolverUpdate)
        {
            skipSolverUpdate = false;
        }
        updateFrame = true;
        if (animatePhysics() && fixTransforms)
        {
            FixTransforms();
        }
    }

    void VRIK::Initiate() {
        if (componentInitiated)
        {
            return;
        }
        FindAnimatorRecursive(transform, true);
        VRIK_InitiateSolver();
        componentInitiated = true;
    }

    void VRIK::LateUpdate() {
        if (skipSolverUpdate)
        {
            return;
        }
        if (!animatePhysics())
        {
            updateFrame = true;
        }
        if (!updateFrame)
        {
            return;
        }
        updateFrame = false;
        VRIK_UpdateSolver();
    }

    void VRIK::OnDisable() {
        Initiate();
    }

    void VRIK::Start() {
        Initiate();
    }

    void VRIK::Update() {
        if (skipSolverUpdate)
        {
            return;
        }
        if (animatePhysics())
        {
            return;
        }
        if (fixTransforms)
        {
            FixTransforms();
        }
    }

    bool VRIK::animatePhysics() {
        if (animator != nullptr)
		{
			return animator->updateMode == UnityEngine::AnimatorUpdateMode::AnimatePhysics;
		}
		return legacy != nullptr && legacy->animatePhysics;
    }

    bool VRIK::isAnimated() {
        return animator != nullptr || legacy != nullptr;
    }

    //IK
    void VRIK::IK_UpdateSolver() {
        if (!GetIKSolver()->initiated)
        {
            VRIK_InitiateSolver();
        }
        if (!GetIKSolver()->initiated)
        {
            return;
        }
        GetIKSolver()->Update();
    }

    void VRIK::IK_InitiateSolver() {
        if (GetIKSolver()->initiated)
        {
            return;
        }
	    GetIKSolver()->Initiate(transform);
    }

    //VRIK
    void VRIK::AutoDetectReferences() {
        references = VRMQavatars::FinalIK::AutoDetectReferences(transform).value_or(nullptr);
    }

    IKSolverVR* VRIK::GetIKSolver() const {
        return solver;
    }

    void VRIK::VRIK_InitiateSolver() {
        if (references->isEmpty())
        {
            AutoDetectReferences();
        }
        if (references->isFilled())
        {
            solver->SetToReferences(references);
        }
        IK_InitiateSolver();
    }

    void VRIK::VRIK_UpdateSolver() {
        if (references->root != nullptr && references->root->localScale.Equals(Sombrero::FastVector3::zero()))
        {
            VRMLogger.error("VRIK Root Transform's scale is zero, can not update VRIK. Make sure you have not calibrated the character to a zero scale.");
            enabled = false;
            return;
        }
        IK_UpdateSolver();
    }

    void VRIK::UpdateSolverExternal() {
        //TODO: check this;
        VRIK_UpdateSolver();
    }

    void VRIK::FixTransforms() const {
        if (!GetIKSolver()->initiated)
        {
            return;
        }
        GetIKSolver()->FixTransforms();
    }
}
