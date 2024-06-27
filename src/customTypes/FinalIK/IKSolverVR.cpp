#include "customTypes/FinalIK/IKSolverVR.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

#include "UnityEngine/Time.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/RaycastHit.hpp"

#include "customTypes/FinalIK/AxisTools.hpp"
#include "customTypes/FinalIK/QuaTools.hpp"

#include "customTypes/FinalIK/V3Tools.hpp"
#include "main.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"

#include "customTypes/FinalIK/MathUtil.hpp"

namespace VRMQavatars::FinalIK {

    void IKSolverVR::FixTransforms() {
        if (!initiated)
        {
            return;
        }
        if (LOD >= 2)
        {
            return;
        }
        for (int i = 1; i < solverTransforms.size(); i++)
        {
            if (solverTransforms[i] != nullptr)
            {
                bool flag = i == 1;
                bool flag2 = i == 8 || i == 9 || i == 12 || i == 13;
                bool flag3 = (i >= 15 && i <= 17) || (i >= 19 && i <= 21);
                if (flag || flag2 || flag3)
                {
                    solverTransforms[i]->localPosition = defaultLocalPositions[i - 1];
                }
                solverTransforms[i]->localRotation = defaultLocalRotations[i - 1];
            }
        }
    }

    Sombrero::FastVector3 IKSolverVR::GetIKPosition() {
        return IKPosition;
    }

    float IKSolverVR::GetIKPositionWeight() {
        return IKPositionWeight;
    }

    UnityEngine::Transform* IKSolverVR::GetRoot() {
        return root;
    }

    void IKSolverVR::Initiate(UnityEngine::Transform* _root) {
        if (OnPreInitiate != nullptr)
        {
            OnPreInitiate();
        }
        if (_root == nullptr)
        {
            VRMLogger.error("Initiating IKSolver with null root Transform.");
        }
        this->root = _root;
        initiated = false;
        std::string empty;
        if (!IsValid(empty))
        {
            return;
        }
        OnInitiate();
        StoreDefaultLocalState();
        initiated = true;
        firstInitiation = false;
        if (OnPostInitiate != nullptr)
        {
            OnPostInitiate();
        }
    }

    bool IKSolverVR::IsValid() {
        std::string empty;
	    return IsValid(empty);
    }

    void IKSolverVR::OnInitialize() {
        UpdateSolverTransforms();
	    Read(readPositions, readRotations, hasChest, hasNeck, hasShoulders, hasToes, hasLegs);
    }

    void IKSolverVR::SetIKPosition(Sombrero::FastVector3 position) {
        IKPosition = position;
    }

    void IKSolverVR::SetIKPositionWeight(float weight) {
        IKPositionWeight = std::clamp(weight, 0.0f, 1.0f);
    }

    void IKSolverVR::Update() {
        if (OnPreUpdate)
        {
            OnPreUpdate();
        }
        if (firstInitiation)
        {
            Initiate(root);
        }
        if (!initiated)
        {
            return;
        }
        OnUpdate();
        if (OnPostUpdate)
        {
            OnPostUpdate();
        }
    }

    Sombrero::FastVector3 IKSolverVR::GetPelvisOffset() {
        if (locomotion->weight <= 0.0f)
        {
            return Sombrero::FastVector3::zero();
        }
        if (locomotion->blockingLayers.value == -1)
        {
            return Sombrero::FastVector3::zero();
        }
        Sombrero::FastVector3 vector = raycastOriginPelvis;
        vector.y = spine->pelvis()->solverPosition.y;
        Sombrero::FastVector3 vector2 = spine->pelvis()->readPosition;
        vector2.y = spine->pelvis()->solverPosition.y;
        Sombrero::FastVector3 vector3 = vector2 - vector;
        UnityEngine::RaycastHit raycastHit;
        if (locomotion->raycastRadius <= 0.0f)
        {
            if (UnityEngine::Physics::Raycast(vector, vector3, byref(raycastHit), vector3.magnitude * 1.0f, locomotion->blockingLayers.value))
            {
                vector2 = raycastHit.point;
            }
        }
        else if (UnityEngine::Physics::SphereCast(vector, locomotion->raycastRadius * 1.0f, vector3, byref(raycastHit), vector3.magnitude, locomotion->blockingLayers.value))
        {
            vector2 = vector + vector3.normalized * raycastHit.distance / 1.0f;
        }
        Sombrero::FastVector3 vector4 = spine->pelvis()->solverPosition;
        vector3 = vector4 - vector2;
        if (locomotion->raycastRadius <= 0.0f)
        {
            if (UnityEngine::Physics::Raycast(vector2, vector3, byref(raycastHit), vector3.magnitude, locomotion->blockingLayers.value))
            {
                vector4 = raycastHit.point;
            }
        }
        else if (UnityEngine::Physics::SphereCast(vector2, locomotion->raycastRadius, vector3, byref(raycastHit), vector3.magnitude, locomotion->blockingLayers.value))
        {
            vector4 = vector2 + vector3.normalized * raycastHit.distance;
        }
        lastOffset = Sombrero::FastVector3::Lerp(lastOffset, Sombrero::FastVector3::zero(), UnityEngine::Time::get_deltaTime() * 3.0f);
        vector4 += ClampMagnitude(lastOffset, 0.75f);
        vector4.y = spine->pelvis()->solverPosition.y;
        lastOffset = Sombrero::FastVector3::Lerp(lastOffset, vector4 - spine->pelvis()->solverPosition, UnityEngine::Time::get_deltaTime() * 15.0f);
        return lastOffset;
    }

    Sombrero::FastVector3 IKSolverVR::GetPosition(int index) {
        return solvedPositions[index];
    }

    Sombrero::FastQuaternion IKSolverVR::GetRotation(int index) {
        return solvedRotations[index];
    }

    void IKSolverVR::GuessHandOrientations(References* references, bool onlyIfZero) {
        if (!references->isFilled())
        {
            VRMLogger.error("VRIK References are not filled in, can not guess hand orientations. Right-click on VRIK header and slect 'Guess Hand Orientations' when you have filled in the references");
            return;
        }
        if (leftArm->wristToPalmAxis == Sombrero::FastVector3::zero() || !onlyIfZero)
        {
            leftArm->wristToPalmAxis = GuessWristToPalmAxis(references->leftHand, references->leftForearm);
        }
        if (leftArm->palmToThumbAxis == Sombrero::FastVector3::zero() || !onlyIfZero)
        {
            leftArm->palmToThumbAxis = GuessPalmToThumbAxis(references->leftHand, references->leftForearm);
        }
        if (rightArm->wristToPalmAxis == Sombrero::FastVector3::zero() || !onlyIfZero)
        {
            rightArm->wristToPalmAxis = GuessWristToPalmAxis(references->rightHand, references->rightForearm);
        }
        if (rightArm->palmToThumbAxis == Sombrero::FastVector3::zero() || !onlyIfZero)
        {
            rightArm->palmToThumbAxis = GuessPalmToThumbAxis(references->rightHand, references->rightForearm);
        }
    }

    float SqrMagnitude(Sombrero::FastVector3 vector) {
		return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
	}

    Sombrero::FastVector3 IKSolverVR::GuessPalmToThumbAxis(UnityEngine::Transform* hand, UnityEngine::Transform* forearm) {
        if (hand->childCount == 0)
        {
            VRMLogger.error("Hand {} does not have any fingers, VRIK can not guess the hand bone's orientation. Please assign 'Wrist To Palm Axis' and 'Palm To Thumb Axis' manually for both arms in VRIK settings.", hand->name);
            return Sombrero::FastVector3::zero();
        }
        float num = std::numeric_limits<float>::infinity();
        int num2 = 0;
        for (int i = 0; i < hand->childCount; i++)
        {
            float num3 = SqrMagnitude(Sombrero::FastVector3(hand->GetChild(i)->position) - hand->position);
            if (num3 < num)
            {
                num = num3;
                num2 = i;
            }
        }
        Sombrero::FastVector3 vector = Sombrero::FastVector3::Cross(Sombrero::FastVector3::Cross(Sombrero::FastVector3(hand->position) - forearm->position, Sombrero::FastVector3(hand->GetChild(num2)->position) - hand->position), Sombrero::FastVector3(hand->position) - forearm->position);
        Sombrero::FastVector3 vector2 = AxisTools::ToVector3(AxisTools::GetAxisToDirection(hand, vector));
        if (Sombrero::FastVector3::Dot(vector, Sombrero::FastQuaternion(hand->rotation) * vector2) < 0.0f)
        {
            vector2 = -vector2;
        }
        return vector2;
    }

    Sombrero::FastVector3 IKSolverVR::GuessWristToPalmAxis(UnityEngine::Transform* hand, UnityEngine::Transform* forearm) {
        Sombrero::FastVector3 vector = Sombrero::FastVector3(forearm->position) - hand->position;
        Sombrero::FastVector3 vector2 = AxisTools::ToVector3(AxisTools::GetAxisToDirection(hand, vector));
        if (Sombrero::FastVector3::Dot(vector, Sombrero::FastQuaternion(hand->rotation) * vector2) > 0.0f)
        {
            vector2 = -vector2;
        }
        return vector2;
    }

    bool IKSolverVR::IsValid(std::string& message) {
        if (solverTransforms.empty() || solverTransforms.size() == 0)
        {
            message = "Trying to initiate IKSolverVR with invalid bone references.";
            return false;
        }
        if (leftArm->wristToPalmAxis == Sombrero::FastVector3::zero())
        {
            message = "Left arm 'Wrist To Palm Axis' needs to be set in VRIK. Please select the hand bone, set it to the axis that points from the wrist towards the palm. If the arrow points away from the palm, axis must be negative.";
            return false;
        }
        if (rightArm->wristToPalmAxis == Sombrero::FastVector3::zero())
        {
            message = "Right arm 'Wrist To Palm Axis' needs to be set in VRIK. Please select the hand bone, set it to the axis that points from the wrist towards the palm. If the arrow points away from the palm, axis must be negative.";
            return false;
        }
        if (leftArm->palmToThumbAxis == Sombrero::FastVector3::zero())
        {
            message = "Left arm 'Palm To Thumb Axis' needs to be set in VRIK. Please select the hand bone, set it to the axis that points from the palm towards the thumb. If the arrow points away from the thumb, axis must be negative.";
            return false;
        }
        if (rightArm->palmToThumbAxis == Sombrero::FastVector3::zero())
        {
            message = "Right arm 'Palm To Thumb Axis' needs to be set in VRIK. Please select the hand bone, set it to the axis that points from the palm towards the thumb. If the arrow points away from the thumb, axis must be negative.";
            return false;
        }
        return true;
    }

    void IKSolverVR::OnInitiate() {
        UpdateSolverTransforms();
        Read(readPositions, readRotations, hasChest, hasNeck, hasShoulders, hasToes, hasLegs);
    }

    void IKSolverVR::OnUpdate() {
        if (IKPositionWeight > 0.0f)
        {
            if (LOD < 2)
            {
                bool flag = false;
                if (lastLOD != LOD && lastLOD == 2)
                {
                    spine->faceDirection = rootBone->readRotation * Sombrero::FastVector3::forward();
                    if (hasLegs)
                    {
                        if (locomotion->weight > 0.0f)
                        {
                            root->position = {spine->headTarget->position.x, root->position.y, spine->headTarget->position.z};
                            auto faceDirection = spine->faceDirection;
                            faceDirection.y = 0.0f;
                            root->rotation = Sombrero::FastQuaternion::LookRotation(faceDirection, Sombrero::FastVector3(root->up));
                            UpdateSolverTransforms();
                            Read(readPositions, readRotations, hasChest, hasNeck, hasShoulders, hasToes, hasLegs);
                            flag = true;
                            locomotion->Reset(readPositions, readRotations);
                        }
                        raycastOriginPelvis = spine->pelvis()->readPosition;
                    }
                }
                if (!flag)
                {
                    UpdateSolverTransforms();
                    Read(readPositions, readRotations, hasChest, hasNeck, hasShoulders, hasToes, hasLegs);
                }
                Solve();
                Write();
                WriteTransforms();
            }
            else if (locomotion->weight > 0.0f)
            {
                root->position = {spine->headTarget->position.x, root->position.y, spine->headTarget->position.z};
                auto vector = Sombrero::FastQuaternion(spine->headTarget->rotation) * spine->anchorRelativeToHead * Sombrero::FastVector3::forward();
                vector.y = 0.0f;
                root->rotation = Sombrero::FastQuaternion::LookRotation(vector, Sombrero::FastVector3(root->up));
            }
        }
        lastLOD = LOD;
    }

    void IKSolverVR::Read(std::vector<Sombrero::FastVector3> positions, std::vector<Sombrero::FastQuaternion> rotations, bool hasChest, bool hasNeck, bool hasShoulders, bool hasToes, bool hasLegs) {
        if (rootBone == nullptr)
        {
            rootBone = new VirtualBone(positions[0], rotations[0]);
        }
        else
        {
            rootBone->Read(positions[0], rotations[0]);
        }
        spine->Read(positions, rotations, hasChest, hasNeck, hasShoulders, hasToes, hasLegs, 0, 1);
        leftArm->Read(positions, rotations, hasChest, hasNeck, hasShoulders, hasToes, hasLegs, hasChest ? 3 : 2, 6);
        rightArm->Read(positions, rotations, hasChest, hasNeck, hasShoulders, hasToes, hasLegs, hasChest ? 3 : 2, 10);
        if (hasLegs)
        {
            leftLeg->Read(positions, rotations, hasChest, hasNeck, hasShoulders, hasToes, hasLegs, 1, 14);
            rightLeg->Read(positions, rotations, hasChest, hasNeck, hasShoulders, hasToes, hasLegs, 1, 18);
        }
        for (int i = 0; i < rotations.size(); i++)
        {
            solvedPositions[i] = positions[i];
            solvedRotations[i] = rotations[i];
        }
        if (!initiated)
        {
            if (hasLegs)
            {
                legs = { leftLeg, rightLeg };
            }
            arms = { leftArm, rightArm };
            if (hasLegs)
            {
                locomotion->Initiate(positions, rotations, hasToes);
            }
            raycastOriginPelvis = spine->pelvis()->readPosition;
            spine->faceDirection = readRotations[0] * Sombrero::FastVector3::forward();
        }
    }

    void IKSolverVR::Reset() {
        
    }

    void IKSolverVR::SetToReferences(References* references) {
        if (!references->isFilled())
        {
            VRMLogger.error("Invalid references, one or more Transforms are missing.");
            return;
        }
        solverTransforms = references->GetTransforms();
        hasChest = solverTransforms[3] != nullptr;
        hasNeck = solverTransforms[4] != nullptr;
        hasShoulders = solverTransforms[6] != nullptr && solverTransforms[10] != nullptr;
        hasToes = solverTransforms[17] != nullptr && solverTransforms[21] != nullptr;
        hasLegs = solverTransforms[14] != nullptr;
        readPositions = std::vector<Sombrero::FastVector3>(solverTransforms.size());
        readRotations = std::vector<Sombrero::FastQuaternion>(solverTransforms.size());
        GuessHandOrientations(references, true);
    }

    void IKSolverVR::Solve() {
        spine->SetLOD(LOD);
        auto& array = arms;
        for (int i = 0; i < array.size(); i++)
        {
            array[i]->SetLOD(LOD);
        }
        if (hasLegs)
        {
            auto array2 = legs;
            for (int i = 0; i < array2.size(); i++)
            {
                array2[i]->SetLOD(LOD);
            }
        }
        spine->PreSolve();
        array = arms;
        for (int i = 0; i < array.size(); i++)
        {
            array[i]->PreSolve();
        }
        if (hasLegs)
        {
            auto array2 = legs;
            for (int i = 0; i < array2.size(); i++)
            {
                array2[i]->PreSolve();
            }
        }
        array = arms;
        for (int i = 0; i < array.size(); i++)
        {
            array[i]->ApplyOffsets();
        }
        spine->ApplyOffsets();
        spine->Solve(rootBone, legs, arms);
        if (hasLegs && locomotion->weight > 0.0f)
        {
            Sombrero::FastVector3 vector = Sombrero::FastVector3::zero();
            Sombrero::FastVector3 vector2 = Sombrero::FastVector3::zero();
            Sombrero::FastQuaternion identity = Sombrero::FastQuaternion::identity();
            Sombrero::FastQuaternion identity2 = Sombrero::FastQuaternion::identity();
            float num = 0.0f;
            float num2 = 0.0f;
            float num3 = 0.0f;
            float num4 = 0.0f;
            locomotion->Solve(rootBone, spine, leftLeg, rightLeg, leftArm, rightArm, supportLegIndex, vector, vector2, identity, identity2, num, num2, num3, num4);
            vector += Sombrero::FastVector3(root->up) * num;
            vector2 += Sombrero::FastVector3(root->up) * num2;
            leftLeg->footPositionOffset += (vector - leftLeg->lastBone()->solverPosition) * IKPositionWeight * (1.0f - leftLeg->positionWeight) * locomotion->weight;
            rightLeg->footPositionOffset += (vector2 - rightLeg->lastBone()->solverPosition) * IKPositionWeight * (1.0f - rightLeg->positionWeight) * locomotion->weight;
            leftLeg->heelPositionOffset += Sombrero::FastVector3(root->up) * num3 * locomotion->weight;
            rightLeg->heelPositionOffset += Sombrero::FastVector3(root->up) * num4 * locomotion->weight;
            Sombrero::FastQuaternion quaternion = QuaTools::FromToRotation(leftLeg->lastBone()->solverRotation, identity);
            Sombrero::FastQuaternion quaternion2 = QuaTools::FromToRotation(rightLeg->lastBone()->solverRotation, identity2);
            quaternion = Sombrero::FastQuaternion::Lerp(Sombrero::FastQuaternion::identity(), quaternion, IKPositionWeight * (1.0f - leftLeg->rotationWeight) * locomotion->weight);
            quaternion2 = Sombrero::FastQuaternion::Lerp(Sombrero::FastQuaternion::identity(), quaternion2, IKPositionWeight * (1.0f - rightLeg->rotationWeight) * locomotion->weight);
            leftLeg->footRotationOffset = quaternion * leftLeg->footRotationOffset;
            rightLeg->footRotationOffset = quaternion2 * rightLeg->footRotationOffset;
            Sombrero::FastVector3 vector3 = Sombrero::FastVector3::Lerp(leftLeg->position + leftLeg->footPositionOffset, rightLeg->position + rightLeg->footPositionOffset, 0.5f);
            vector3 = V3Tools::PointToPlane(vector3, rootBone->solverPosition, Sombrero::FastVector3(root->up));
            Sombrero::FastVector3 vector4 = rootBone->solverPosition + rootVelocity * UnityEngine::Time::get_deltaTime() * 2.0f * locomotion->weight;
            vector4 = Sombrero::FastVector3::Lerp(vector4, vector3, UnityEngine::Time::get_deltaTime() * locomotion->rootSpeed * locomotion->weight);
            rootBone->solverPosition = vector4;
            rootVelocity += (vector3 - rootBone->solverPosition) * UnityEngine::Time::get_deltaTime() * 10.0f;
            Sombrero::FastVector3 vector5 = V3Tools::ExtractVertical(rootVelocity, Sombrero::FastVector3(root->up), 1.0f);
            rootVelocity -= vector5;
            float num5 = num + num2;
            bodyOffset = Sombrero::FastVector3::Lerp(bodyOffset, Sombrero::FastVector3(root->up) * num5, UnityEngine::Time::get_deltaTime() * 3.0f);
            bodyOffset = Sombrero::FastVector3::Lerp(Sombrero::FastVector3::zero(), bodyOffset, locomotion->weight);
        }
        if (hasLegs)
        {
            auto& array2 = legs;
            for (int i = 0; i < array2.size(); i++)
            {
                array2[i]->ApplyOffsets();
            }
            if (!plantFeet || LOD > 0)
            {
                spine->InverseTranslateToHead(legs, false, false, bodyOffset, 1.0f);
                array2 = legs;
                for (int i = 0; i < array2.size(); i++)
                {
                    array2[i]->TranslateRoot(spine->pelvis()->solverPosition, spine->pelvis()->solverRotation);
                }
                array2 = legs;
                for (int i = 0; i < array2.size(); i++)
                {
                    array2[i]->Solve(true);
                }
            }
            else
            {
                for (int j = 0; j < 2; j++)
                {
                    spine->InverseTranslateToHead(legs, true, true, bodyOffset, 1.0f);
                    array2 = legs;
                    for (int i = 0; i < array2.size(); i++)
                    {
                        array2[i]->TranslateRoot(spine->pelvis()->solverPosition, spine->pelvis()->solverRotation);
                    }
                    array2 = legs;
                    for (int i = 0; i < array2.size(); i++)
                    {
                        array2[i]->Solve(j == 0);
                    }
                }
            }
        }
        else
        {
            spine->InverseTranslateToHead(legs, false, false, bodyOffset, 1.0f);
        }
        for (int k = 0; k < arms.size(); k++)
        {
            arms[k]->TranslateRoot(spine->chest()->solverPosition, spine->chest()->solverRotation);
        }
        for (int l = 0; l < arms.size(); l++)
        {
            arms[l]->Solve(l == 0);
        }
        spine->ResetOffsets();
        if (hasLegs)
        {
            auto& array2 = legs;
            for (int i = 0; i < array2.size(); i++)
            {
                array2[i]->ResetOffsets();
            }
        }
        array = arms;
        for (int i = 0; i < array.size(); i++)
        {
            array[i]->ResetOffsets();
        }
        if (hasLegs)
        {
            spine->pelvisPositionOffset += GetPelvisOffset();
            spine->chestPositionOffset += spine->pelvisPositionOffset;
        }
        Write();
        if (hasLegs)
        {
            supportLegIndex = -1;
            float num6 = std::numeric_limits<float>::infinity();
            for (int m = 0; m < legs.size(); m++)
            {
                float num7 = SqrMagnitude(legs[m]->lastBone()->solverPosition - legs[m]->bones[0]->solverPosition);
                if (num7 < num6)
                {
                    supportLegIndex = m;
                    num6 = num7;
                }
            }
        }
    }

    void IKSolverVR::StoreDefaultLocalState() {
        for (int i = 1; i < solverTransforms.size(); i++)
        {
            if (solverTransforms[i] != nullptr)
            {
                defaultLocalPositions[i - 1] = solverTransforms[i]->localPosition;
                defaultLocalRotations[i - 1] = solverTransforms[i]->localRotation;
            }
        }
    }

    void IKSolverVR::UpdateSolverTransforms() {
        for (int i = 0; i < solverTransforms.size(); i++)
        {
            if (solverTransforms[i] != nullptr)
            {
                readPositions[i] = solverTransforms[i]->position;
                readRotations[i] = solverTransforms[i]->rotation;
            }
        }
    }

    void IKSolverVR::Write() {
        solvedPositions[0] = rootBone->solverPosition;
        solvedRotations[0] = rootBone->solverRotation;
        spine->Write(solvedPositions, solvedRotations);
        if (hasLegs)
        {
            auto array = legs;
            for (int i = 0; i < array.size(); i++)
            {
                array[i]->Write(solvedPositions, solvedRotations);
            }
        }
        auto array2 = arms;
        for (int i = 0; i < array2.size(); i++)
        {
            array2[i]->Write(solvedPositions, solvedRotations);
        }
    }

    void IKSolverVR::WriteTransforms() {
        for (int i = 0; i < solverTransforms.size(); i++)
        {
            if (solverTransforms[i] != nullptr)
            {
                bool flag = i < 2;
                bool flag2 = i == 8 || i == 9 || i == 12 || i == 13;
                bool flag3 = (i >= 15 && i <= 17) || (i >= 19 && i <= 21);
                if (LOD > 0)
                {
                    flag2 = false;
                    flag3 = false;
                }
                if (flag)
                {
                    solverTransforms[i]->position = V3Tools::Lerp(solverTransforms[i]->position, GetPosition(i), IKPositionWeight);
                }
                if (flag2 || flag3)
                {
                    if (IKPositionWeight < 1.0f)
                    {
                        auto localPosition = solverTransforms[i]->localPosition;
                        solverTransforms[i]->position = V3Tools::Lerp(solverTransforms[i]->position, GetPosition(i), IKPositionWeight);
                        solverTransforms[i]->localPosition = Sombrero::FastVector3::Project(solverTransforms[i]->localPosition, localPosition);
                    }
                    else
                    {
                        solverTransforms[i]->position = V3Tools::Lerp(solverTransforms[i]->position, GetPosition(i), IKPositionWeight);
                    }
                }
                solverTransforms[i]->rotation = QuaTools::Lerp(solverTransforms[i]->rotation, GetRotation(i), IKPositionWeight);
            }
        }
    }
}