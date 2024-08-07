#pragma once

#include "Point.hpp"
#include "References.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Keyframe.hpp"

#include "VR/Spine.hpp"
#include "VR/VirtualBone.hpp"
#include "VR/Arm.hpp"
#include "VR/Leg.hpp"
#include "VR/Locomotion.hpp"

#include "sombrero/shared/QuaternionUtils.hpp"
#include "sombrero/shared/Vector3Utils.hpp"

namespace VRMQavatars::FinalIK {
    class IKSolverVR {
    public:
        virtual ~IKSolverVR() = default;

        // IKSolver

        void FixTransforms();
        Sombrero::FastVector3 GetIKPosition();
        float GetIKPositionWeight();
        UnityEngine::Transform* GetRoot();
        void Initiate(UnityEngine::Transform* root);
        bool IsValid();
        void OnInitialize();
        void SetIKPosition(Sombrero::FastVector3 position);
        void SetIKPositionWeight(float weight);
        void Update();

        bool initiated;
        bool firstInitiation;
        Sombrero::FastVector3 IKPosition;
        float IKPositionWeight = 1.0f;
        std::function<void()> OnPostInitiate;
        std::function<void()> OnPostUpdate;
        std::function<void()> OnPreInitiate;
        std::function<void()> OnPreUpdate;
        UnityEngine::Transform* root;

        // IKSolverVR

        Sombrero::FastVector3 GetPelvisOffset();
        Sombrero::FastVector3 GetPosition(int index);
        Sombrero::FastQuaternion GetRotation(int index);
        void GuessHandOrientations(References* references, bool onlyIfZero);
        Sombrero::FastVector3 GuessPalmToThumbAxis(UnityEngine::Transform* hand, UnityEngine::Transform* forearm);
        Sombrero::FastVector3 GuessWristToPalmAxis(UnityEngine::Transform* hand, UnityEngine::Transform* forearm);
        bool IsValid(std::string& message);
        void OnInitiate();
        void OnUpdate();
        void Read(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs);
        void Reset();
        void SetToReferences(References* references);
        void Solve();
        void StoreDefaultLocalState();
        void UpdateSolverTransforms();
        void Write();
        void WriteTransforms();

        std::vector<UnityEngine::Transform*> solverTransforms = std::vector<UnityEngine::Transform*>(0);
		bool hasChest = false;
		bool hasNeck = false;
		bool hasShoulders = false;
		bool hasToes = false;
		bool hasLegs = false;
		std::vector<Sombrero::FastVector3> readPositions = std::vector<Sombrero::FastVector3>(0);
		std::vector<Sombrero::FastQuaternion> readRotations = std::vector<Sombrero::FastQuaternion>(0);
		std::vector<Sombrero::FastVector3> solvedPositions = std::vector<Sombrero::FastVector3>(22);
		std::vector<Sombrero::FastQuaternion> solvedRotations = std::vector<Sombrero::FastQuaternion>(22);
		std::vector<Sombrero::FastQuaternion> defaultLocalRotations = std::vector<Sombrero::FastQuaternion>(21);
		std::vector<Sombrero::FastVector3> defaultLocalPositions = std::vector<Sombrero::FastVector3>(21);
		Sombrero::FastVector3 rootVelocity = Sombrero::FastVector3::zero();
		Sombrero::FastVector3 bodyOffset = Sombrero::FastVector3::zero();
		int supportLegIndex = 0;
		int lastLOD = 0;
		int LOD = 0;
		bool plantFeet = true;
		VirtualBone* rootBone;
		Spine* spine = new Spine();
		Arm* leftArm = new Arm();
		Arm* rightArm = new Arm();
		Leg* leftLeg = new Leg();
		Leg* rightLeg = new Leg();
		Locomotion* locomotion = new Locomotion();
		std::vector<Leg*> legs = std::vector<Leg*>(2);
		std::vector<Arm*> arms = std::vector<Arm*>(2);
		Sombrero::FastVector3 raycastOriginPelvis = Sombrero::FastVector3::zero();
		Sombrero::FastVector3 lastOffset = Sombrero::FastVector3::zero();
    };
}