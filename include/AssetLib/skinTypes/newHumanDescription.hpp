#pragma once
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/byref.hpp"
#include "System/ValueType.hpp"
#include "newHumanBone.hpp"
#include "UnityEngine/SkeletonBone.hpp"
#include "beatsaber-hook/shared/utils/typedefs-array.hpp"
#include "beatsaber-hook/shared/utils/typedefs-string.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-type-check.hpp"

struct CustomHumanDescription {
    ArrayW<CustomHumanBone> human;
    ArrayW<::UnityEngine::SkeletonBone> skeleton;
    float m_ArmTwist;
    float m_ForeArmTwist;
    float m_UpperLegTwist;
    float m_LegTwist;
    float m_ArmStretch;
    float m_LegStretch;
    float m_FeetSpacing;
    float m_GlobalScale;
    StringW m_RootMotionBoneName;
    bool m_HasTranslationDoF;
    bool m_HasExtraRoot;
    bool m_SkeletonHasParents;

    constexpr CustomHumanDescription(ArrayW<CustomHumanBone> human_, ArrayW<UnityEngine::SkeletonBone> skeleton_, float m_ArmTwist_, float m_ForeArmTwist_, float m_UpperLegTwist_ = {}, float m_LegTwist_ = {}, float m_ArmStretch_ = {}, float m_LegStretch_ = {}, float m_FeetSpacing_ = {}, float m_GlobalScale_ = {}, ::StringW m_RootMotionBoneName_ = {}, bool m_HasTranslationDoF_ = {}, bool m_HasExtraRoot_ = {}, bool m_SkeletonHasParents_ = {}) noexcept : human{human_}, skeleton{skeleton_}, m_ArmTwist{m_ArmTwist_}, m_ForeArmTwist{m_ForeArmTwist_}, m_UpperLegTwist{m_UpperLegTwist_}, m_LegTwist{m_LegTwist_}, m_ArmStretch{m_ArmStretch_}, m_LegStretch{m_LegStretch_}, m_FeetSpacing{m_FeetSpacing_}, m_GlobalScale{m_GlobalScale_}, m_RootMotionBoneName{m_RootMotionBoneName_}, m_HasTranslationDoF{m_HasTranslationDoF_}, m_HasExtraRoot{m_HasExtraRoot_}, m_SkeletonHasParents{m_SkeletonHasParents_} {}
    CustomHumanDescription() = default;
};
static_assert(sizeof(CustomHumanDescription) == 0x40);
DEFINE_IL2CPP_ARG_TYPE(CustomHumanDescription, "UnityEngine", "HumanDescription");