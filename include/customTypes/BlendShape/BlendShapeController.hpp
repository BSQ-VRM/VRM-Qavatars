#pragma once

#include <sombrero/shared/FastVector3.hpp>
#include <UnityEngine/SkinnedMeshRenderer.hpp>

#include "AssetLib/structure/VRM/VRMBlendShapeMaster.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "custom-types/shared/coroutine.hpp"

namespace VRMQavatars::BlendShape {
    class BlendShapeController : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.BlendShape", BlendShapeController, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(BlendShapeController);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(BlendShapeController);
    public:

        DECLARE_INSTANCE_METHOD(void, Init);
        DECLARE_INSTANCE_METHOD(void, OnDestroy);
        DECLARE_INSTANCE_METHOD(void, Update);
    public:
        custom_types::Helpers::Coroutine AutoBlinkCoroutine();

        void SetBlendshape(int preset, float value);

        std::vector<UnityEngine::SkinnedMeshRenderer*> renderers;

        std::map<AssetLib::Structure::VRM::BlendShapePreset, AssetLib::Structure::VRM::VRMBlendShapeGroup> blendShapePresetMappings;
        std::map<int, AssetLib::Structure::VRM::VRMBlendShapeGroup> blendShapeMappings;
        std::map<std::string, int> reverseMappings;

        std::map<int, float> blendShapeValues;
        std::map<int, float> blendShapeTargetValues;

        bool init = false;
        bool allowAutoBlink = true;

        UnityEngine::Transform* leftEye = nullptr;
        UnityEngine::Transform* rightEye = nullptr;
        Sombrero::FastVector3 eyeVec = Sombrero::FastVector3::zero();
        float time;
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::BlendShape::BlendShapeController);