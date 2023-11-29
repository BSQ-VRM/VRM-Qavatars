#pragma once
#include <System/String.hpp>
#include <UnityEngine/Shader.hpp>

namespace DynamicShadowProjector
{
    enum TextureMultiSample {
        x1 = 1,
        x2 = 2,
        x4 = 4,
        x8 = 8,
    };
    enum TextureSuperSample {
        sx1 = 1,
        sx4 = 4,
        sx16 = 16,
    };
    enum UpdateFunction {
        OnPreCull = 0,
        LateUpdate,
        UpdateTransform,
    };
}
