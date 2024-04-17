#pragma once

#include "BodyPart.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/AnimationCurve.hpp"
#include "VirtualBone.hpp"

namespace VRMQavatars::FinalIK {
    enum ShoulderRotationMode
	{
		YawPitch,
		FromTo
	};

    class Arm : public BodyPart {
    public:
        virtual ~Arm() = default;
        Arm();
        
        void ApplyOffsets() override;
        float DamperValue(float value, float min, float max, float weight = 1.0f);
        Sombrero::FastVector3 GetBendNormal(Sombrero::FastVector3 dir);
        void OnRead(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs, int rootIndex, int index) override;
        void PreSolve() override;
        void ResetOffsets() override;
        void Solve(bool isLeft);
        void Stretching();
        void Write(std::vector<Sombrero::FastVector3>& positions, std::vector<Sombrero::FastQuaternion>& rotations) override;

        VirtualBone* shoulder() { return bones[0]; }
        VirtualBone* upperArm() { return bones[hasShoulder ? 1 : 0]; }
        VirtualBone* forearm() { return bones[hasShoulder ? 2 : 1]; }
        VirtualBone* hand() { return bones[hasShoulder ? 3 : 2]; }

        UnityEngine::Transform* target;
        UnityEngine::Transform* bendGoal;
        
        float armLengthMlp = 1.f;
        float positionWeight = 1.f;
        float rotationWeight = 1.f;
        ShoulderRotationMode shoulderRotationMode;
        float shoulderRotationWeight = 1.f;
        float shoulderTwistWeight = 1.f;
        float bendGoalWeight;
        float swivelOffset;
        Sombrero::FastVector3 wristToPalmAxis = Sombrero::FastVector3::zero();
        Sombrero::FastVector3 palmToThumbAxis = Sombrero::FastVector3::zero();
        UnityEngine::AnimationCurve* stretchCurve;
        Sombrero::FastVector3 IKPosition;
        Sombrero::FastQuaternion IKRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 bendDirection = Sombrero::FastVector3::back();
        Sombrero::FastVector3 handPositionOffset;
        Sombrero::FastVector3 position;
        Sombrero::FastQuaternion rotation;
        bool hasShoulder;
        Sombrero::FastVector3 chestForwardAxis;
        Sombrero::FastVector3 chestUpAxis;
        Sombrero::FastQuaternion chestRotation = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 chestForward;
        Sombrero::FastVector3 chestUp;
        Sombrero::FastQuaternion forearmRelToUpperArm = Sombrero::FastQuaternion::identity();
        Sombrero::FastVector3 upperArmBendAxis;
        const float yawOffsetAngle = 45.f;
        const float pitchOffsetAngle = -30.f;
    };
}