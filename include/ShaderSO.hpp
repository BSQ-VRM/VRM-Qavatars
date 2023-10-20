#pragma once

#include "main.hpp"
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/ScriptableObject.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/GameObject.hpp"

DECLARE_CLASS_CODEGEN(VRMData, ShaderSO, UnityEngine::ScriptableObject,
    DECLARE_INSTANCE_FIELD(UnityEngine::Shader*, mToonShader);
    DECLARE_INSTANCE_FIELD(UnityEngine::Shader*, unlitShader);
    DECLARE_INSTANCE_FIELD(UnityEngine::Material*, body);
    DECLARE_INSTANCE_FIELD(UnityEngine::Material*, hair);
    DECLARE_INSTANCE_FIELD(UnityEngine::Material*, face);
    DECLARE_INSTANCE_FIELD(UnityEngine::Material*, detail);
)