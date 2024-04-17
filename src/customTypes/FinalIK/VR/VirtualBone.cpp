#include "customTypes/FinalIK/VR/VirtualBone.hpp"

namespace VRMQavatars::FinalIK {
    void VirtualBone::Read(Sombrero::FastVector3 position, Sombrero::FastQuaternion rotation) {
    
    }

    void VirtualBone::SwingRotation(std::vector<VirtualBone*> bones, int index, Sombrero::FastVector3 swingTarget, float weight) {

    }

    float VirtualBone::PreSolve(std::vector<VirtualBone*>& bones) {

    }
    
    void VirtualBone::RotateAroundPoint(std::vector<VirtualBone*> bones, int index, Sombrero::FastVector3 point, Sombrero::FastQuaternion rotation) {

    }
    
    void VirtualBone::RotateBy(std::vector<VirtualBone*> bones, int index, Sombrero::FastQuaternion rotation) {

    }
    
    void VirtualBone::RotateBy(std::vector<VirtualBone*> bones, Sombrero::FastQuaternion rotation) {

    }
    
    void VirtualBone::RotateTo(std::vector<VirtualBone*> bones, int index, Sombrero::FastQuaternion rotation) {

    }
    
    void VirtualBone::SolveTrigonometric(std::vector<VirtualBone*> bones, int first, int second, int third, Sombrero::FastVector3 targetPosition, Sombrero::FastVector3 bendNormal, float weight) {

    }
    
    Sombrero::FastVector3 VirtualBone::GetDirectionToBendPoint(Sombrero::FastVector3 direction, float directionMag, Sombrero::FastVector3 bendDirection, float sqrMag1, float sqrMag2) {

    }
    
    void VirtualBone::SolveFABRIK(std::vector<VirtualBone*> bones, Sombrero::FastVector3 startPosition, Sombrero::FastVector3 targetPosition, float weight, float minNormalizedTargetDistance, int iterations, float length, Sombrero::FastVector3 startOffset) {

    }
    
    Sombrero::FastVector3 VirtualBone::SolveFABRIKJoint(Sombrero::FastVector3 pos1, Sombrero::FastVector3 pos2, float length) {

    }
    
    void VirtualBone::SolveCCD(std::vector<VirtualBone*> bones, Sombrero::FastVector3 targetPosition, float weight, int iterations) {

    }
}