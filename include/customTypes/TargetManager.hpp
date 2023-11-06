#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

DECLARE_CLASS_CODEGEN(MyNamespace, Counter, UnityEngine::MonoBehaviour,
    // DECLARE_INSTANCE_METHOD creates methods
    DECLARE_INSTANCE_METHOD(void, Update);

    // DECLARE_INSTANCE_FIELD creates fields
    DECLARE_INSTANCE_FIELD(int, counts);
)