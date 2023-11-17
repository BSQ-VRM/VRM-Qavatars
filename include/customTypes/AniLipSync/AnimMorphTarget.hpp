#pragma once

#include "LowLatencyLipSyncContext.hpp"
#include "customTypes/BlendShape/BlendShapeController.hpp"

namespace VRMQavatars::AniLipSync {
    class AnimMorphTarget : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.AniLipSync", AnimMorphTarget, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(AnimMorphTarget);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(AnimMorphTarget);
    public:

        DECLARE_INSTANCE_METHOD(void, Start);
        DECLARE_INSTANCE_METHOD(void, Update);
        DECLARE_INSTANCE_METHOD(void, ResetLipSyncBlendShapes);

        DECLARE_INSTANCE_FIELD(BlendShape::BlendShapeController*, controller);
        DECLARE_INSTANCE_FIELD(OVRLipSync::OVRLipSyncContextBase*, context);
        DECLARE_INSTANCE_FIELD(AniLipSync::LowLatencyLipSyncContext*, lowLatencyLipSyncContext);

        int frameRate = 30;
        std::vector<AssetLib::Structure::VRM::BlendShapePreset> viseme2BlendShape = {
            AssetLib::Structure::VRM::BlendShapePreset::A,
            AssetLib::Structure::VRM::BlendShapePreset::E,
            AssetLib::Structure::VRM::BlendShapePreset::I,
            AssetLib::Structure::VRM::BlendShapePreset::O,
            AssetLib::Structure::VRM::BlendShapePreset::U,
        };
        float threshold = 2.0f;

        float frameRateTimer;
        ovrLipSyncViseme previousViseme;
        float rmsThreshold = 0.005f;
        float transitionTimer;
        ovrLipSyncFrame updateFrame;
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::AniLipSync::AnimMorphTarget);