#include "LightManager.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Light.hpp"
#include "UnityEngine/LightType.hpp"
#include "UnityEngine/Transform.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

namespace VRMQavatars {
    SafePtrUnity<UnityEngine::Light> LightManager::_light;

    void LightManager::Init()
    {
        _light = UnityEngine::GameObject::New_ctor()->AddComponent<UnityEngine::Light*>();
        _light.ptr()->set_intensity(1.0f);
        static auto setType = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, UnityEngine::LightType>("UnityEngine.Light::set_type");
        setType(_light.ptr(), UnityEngine::LightType::Directional);

        UnityEngine::GameObject::DontDestroyOnLoad(_light.ptr()->get_gameObject());
    }

    void LightManager::SetGlobalLightIntensity(float val)
    {
        if(_light.isAlive() && _light.isHandleValid())
        {
            _light.ptr()->set_intensity(val);
        }
    }

    void LightManager::SetGlobalLightColor(UnityEngine::Color val)
    {
        if(_light.isAlive() && _light.isHandleValid())
        {
            _light.ptr()->set_color(val);
        }
    }

    void LightManager::SetGlobalLightRotation(UnityEngine::Vector3 val)
    {
        if(_light.isAlive() && _light.isHandleValid())
        {
            _light.ptr()->get_transform()->set_rotation(UnityEngine::Quaternion::Euler(val));
        }
    }
}
