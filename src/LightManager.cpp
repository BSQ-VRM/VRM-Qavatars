#include "LightManager.hpp"

#include "sceneEventManager.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Light.hpp"
#include "UnityEngine/LightType.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "GlobalNamespace/DirectionalLight.hpp"
#include "GlobalNamespace/PointLight.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "config/ConfigManager.hpp"

namespace VRMQavatars {
    CP_SDK_IL2CPP_INHERIT_INIT(LightManager);

    bool LightManager::inGame;

    SafePtrUnity<UnityEngine::Light> LightManager::_light;
    SafePtrUnity<UnityEngine::Light> LightManager::_beatmapLight;
    SafePtrUnity<UnityEngine::Light> LightManager::_platform;
    SafePtrUnity<UnityEngine::Light> LightManager::_saberLight1;
    SafePtrUnity<UnityEngine::Light> LightManager::_saberLight2;

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(LightManager)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(LightManager)
    {

    }

    //Proxy Funcs
    void LightManager::Awake()
    {
        Init();

        SceneEventManager::OnGameEnter += CP_SDK::Utils::Action<>([]
        {
            inGame = true;1
        });

        UnityEngine::GameObject::DontDestroyOnLoad(get_gameObject());
    }

    void LightManager::Update()
    {
        UpdateLights();
    }

    //Create Lights
    void LightManager::Init()
    {
        //Global Light
        {
            _light = UnityEngine::GameObject::New_ctor("Global VRM Light")->AddComponent<UnityEngine::Light*>();
            static auto setType = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, UnityEngine::LightType>("UnityEngine.Light::set_type");
            setType(_light.ptr(), UnityEngine::LightType::Directional);

            UnityEngine::GameObject::DontDestroyOnLoad(_light.ptr()->get_gameObject());
        }

        //Beatmap Light
        {
            _beatmapLight = UnityEngine::GameObject::New_ctor("Beatmap Lighting")->AddComponent<UnityEngine::Light*>();
            static auto setType = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, UnityEngine::LightType>("UnityEngine.Light::set_type");
            setType(_beatmapLight.ptr(), UnityEngine::LightType::Directional);

            UnityEngine::GameObject::DontDestroyOnLoad(_beatmapLight.ptr()->get_gameObject());

            _beatmapLight->
        }

        UpdateLightValues();
    }

    void LightManager::UpdateLightValues()
    {
        if(_light.isAlive() && _light.isHandleValid())
        {
            _light.ptr()->set_intensity(Config::ConfigManager::GetLightingSettings().globalLightIntensity);
            _light.ptr()->set_color(Config::ConfigManager::GetLightingSettings().globalColor);
            _light.ptr()->get_transform()->set_rotation(UnityEngine::Quaternion::Euler(Config::ConfigManager::GetLightingSettings().lightRotation));
        }
    }
}
