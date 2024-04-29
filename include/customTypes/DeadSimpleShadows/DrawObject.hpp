#pragma once

#include "System/Collections/Generic/List_1.hpp"
#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>
#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/Renderer.hpp>

namespace DeadSimpleShadows
{
    class DrawObject : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("DeadSimpleShadows", DrawObject, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(DrawObject);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(DrawObject);
    public:

        DECLARE_INSTANCE_FIELD(System::Collections::Generic::List_1<UnityEngine::Renderer*>*, excludedMeshRenderers);
        DECLARE_INSTANCE_FIELD(System::Collections::Generic::List_1<UnityEngine::Renderer*>*, responsibleMeshRenderers);
        DECLARE_INSTANCE_METHOD(void, CheckChildren);
        DECLARE_INSTANCE_METHOD(void, OnEnable);
        DECLARE_INSTANCE_METHOD(void, Start);
        DECLARE_INSTANCE_METHOD(void, OnDisable);
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(DeadSimpleShadows::DrawObject);
