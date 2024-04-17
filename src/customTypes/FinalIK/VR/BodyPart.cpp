#include "customTypes/FinalIK/VR/BodyPart.hpp"
#include "customTypes/FinalIK/QuaTools.hpp"

namespace VRMQavatars::FinalIK {

    void BodyPart::MovePosition(Sombrero::FastVector3 position) {
        auto vector = position - bones[0]->solverPosition;
        auto& array = bones;
        for (int i = 0; i < array.size(); i++)
        {
            array[i]->solverPosition += vector;
        }
    }

    void BodyPart::MoveRotation(Sombrero::FastQuaternion rotation) {
        auto quaternion = QuaTools::FromToRotation(bones[0]->solverRotation, rotation);
	    VirtualBone::RotateAroundPoint(bones, 0, bones[0]->solverPosition, quaternion);
    }

    void BodyPart::Read(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs, int rootIndex, int index) {
    	index = index;
        rootPosition = positions[rootIndex];
        rootRotation = rotations[rootIndex];
        OnRead(positions, rotations, hasChest, hasNeck, hasShoulders, hasToes, hasLegs, rootIndex, index);
        mag = VirtualBone::PreSolve(bones);
        sqrMag = mag * mag;
        initiated = true;
    }

    void BodyPart::RotateTo(VirtualBone* bone, Sombrero::FastQuaternion rotation, float weight) {
        if (weight <= 0.0f)
        {
            return;
        }
        auto quaternion = QuaTools::FromToRotation(bone->solverRotation, rotation);
        if (weight < 1.0f)
        {
            quaternion = Sombrero::FastQuaternion::Slerp(Sombrero::FastQuaternion::identity(), quaternion, weight);
        }
        for (int i = 0; i < bones.size(); i++)
        {
            if (bones[i] == bone)
            {
                VirtualBone::RotateAroundPoint(bones, i, bones[i]->solverPosition, quaternion);
                return;
            }
        }
    }

    void BodyPart::SetLOD(int lod) {
        LOD = lod;
    }

    void BodyPart::TranslateRoot(Sombrero::FastVector3 newRootPos, Sombrero::FastQuaternion newRootRot) {
        Sombrero::FastVector3 vector = newRootPos - rootPosition;
        rootPosition = newRootPos;
        auto& array = bones;
        for (int i = 0; i < array.size(); i++)
        {
            array[i]->solverPosition += vector;
        }
        auto quaternion = QuaTools::FromToRotation(rootRotation, newRootRot);
        rootRotation = newRootRot;
        VirtualBone::RotateAroundPoint(bones, 0, newRootPos, quaternion);
    }
}