#pragma once

#include <UnityEngine/Material.hpp>

#include "main.hpp"
#include "UnityEngine/Behaviour.hpp"

#define DEFINE_ICALL_PROP(namespace, prop, type) \
type get_ ## prop() { \
    static auto prop = reinterpret_cast<function_ptr_t< ## type, Projector*>>(il2cpp_functions::resolve_icall((#namespace + std::string("get_") + #prop).c_str())); \
    return prop(this); \
} \
void set_ ## prop(type value) { \
    static auto prop = reinterpret_cast<function_ptr_t<void, Projector*, ## type>>(il2cpp_functions::resolve_icall((#namespace + std::string("set_") + #prop).c_str())); \
    prop(this, value); \
} \

namespace VRMQavatars {
    class Projector : public UnityEngine::Behaviour
    {
    public:
        DEFINE_ICALL_PROP(UnityEngine.Projector::, nearClipPlane, float)
        DEFINE_ICALL_PROP(UnityEngine.Projector::, farClipPlane, float)
        DEFINE_ICALL_PROP(UnityEngine.Projector::, fieldOfView, float)
        DEFINE_ICALL_PROP(UnityEngine.Projector::, aspectRatio, float)
        DEFINE_ICALL_PROP(UnityEngine.Projector::, orthographic, bool)
        DEFINE_ICALL_PROP(UnityEngine.Projector::, orthographicSize, float)
        DEFINE_ICALL_PROP(UnityEngine.Projector::, ignoreLayers, int)
        DEFINE_ICALL_PROP(UnityEngine.Projector::, material, UnityEngine::Material*)
    };
}
DEFINE_IL2CPP_ARG_TYPE(VRMQavatars::Projector*, "UnityEngine", "Projector");