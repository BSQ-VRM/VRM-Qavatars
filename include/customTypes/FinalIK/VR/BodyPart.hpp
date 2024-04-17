#pragma once

#include "VirtualBone.hpp"

namespace VRMQavatars::FinalIK {
    class BodyPart {
    public:
        BodyPart();

        virtual void ApplyOffsets() = 0;
        void MovePosition(Sombrero::FastVector3 position);
        void MoveRotation(Sombrero::FastQuaternion rotation);
        virtual void OnRead(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs, int rootIndex, int index) = 0;
        virtual void PreSolve() = 0;
        void Read(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs, int rootIndex, int index);
        virtual void ResetOffsets() = 0;
        void RotateTo(VirtualBone* bone, Sombrero::FastQuaternion rotation, float weight = 1.0f);
        void SetLOD(int lod);
        void TranslateRoot(Sombrero::FastVector3 newRootPos, Sombrero::FastQuaternion newRootRot);
        virtual void Write(std::vector<Sombrero::FastVector3>& positions, std::vector<Sombrero::FastQuaternion>& rotations) = 0;

        float sqrMag;
        float mag;
        std::vector<VirtualBone*> bones = std::vector<VirtualBone*>();
        bool initiated;
        Sombrero::FastVector3 rootPosition;
        Sombrero::FastQuaternion rootRotation = Sombrero::FastQuaternion::identity();
        int index = -1;
        int LOD;
    };
}