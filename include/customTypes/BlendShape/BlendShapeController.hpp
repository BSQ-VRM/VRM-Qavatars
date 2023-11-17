#pragma once

#include <UnityEngine/SkinnedMeshRenderer.hpp>

#include "AssetLib/structure/VRM/VRMBlendShapeMaster.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

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

        DECLARE_INSTANCE_FIELD(UnityEngine::SkinnedMeshRenderer*, headRenderer);

        void SetBlendshape(AssetLib::Structure::VRM::BlendShapePreset preset, float value);

        std::map<AssetLib::Structure::VRM::BlendShapePreset, AssetLib::Structure::VRM::VRMBlendShapeGroup> blendShapeMappings;

        bool init;
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::BlendShape::BlendShapeController);