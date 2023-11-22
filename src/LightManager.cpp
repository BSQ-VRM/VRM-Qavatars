#include "LightManager.hpp"

#include "sceneEventManager.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Light.hpp"
#include "UnityEngine/LightType.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "UnityEngine/WaitForSeconds.hpp"

#include "GlobalNamespace/DirectionalLight.hpp"
#include "GlobalNamespace/PointLight.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberModelController.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "config/ConfigManager.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/Unity/MTCoroutineStarter.hpp"

namespace VRMQavatars {
    CP_SDK_IL2CPP_INHERIT_INIT(LightManager);

    bool LightManager::inGame = false;

    SafePtrUnity<UnityEngine::Light> LightManager::_light;
    SafePtrUnity<UnityEngine::Light> LightManager::_platform;

    SafePtrUnity<GlobalNamespace::SaberManager> LightManager::_saberManager;
    SafePtr<GlobalNamespace::ColorManager> LightManager::_colorManager;

    SafePtrUnity<UnityEngine::Light> LightManager::_saberLight1;
    SafePtrUnity<UnityEngine::Light> LightManager::_saberLight2;

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(LightManager)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(LightManager)
    {

    }

    custom_types::Helpers::Coroutine LightManager::GrabColorManager()
    {
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(0.1f));
        while(!_colorManager || _colorManager.ptr() == nullptr)
        {
            getLogger().info("finding ColorManager");
            _colorManager = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::SaberModelController*>().FirstOrDefault()->colorManager;
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForEndOfFrame::New_ctor());
        }
        co_return;
    }

    //Proxy Funcs
    void LightManager::Awake()
    {
        UnityEngine::GameObject::DontDestroyOnLoad(get_gameObject());

        Init();

        SceneEventManager::OnGameEnter += CP_SDK::Utils::Action<>([]
        {
            getLogger().info("hi game");
            if(!_saberManager || _saberManager.ptr() == nullptr)
            {
                getLogger().info("finding saber manager");
                _saberManager = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::SaberManager*>().FirstOrDefault();
                getLogger().info("found %p", _saberManager.ptr());
                CP_SDK::Unity::MTCoroutineStarter::Start(GrabColorManager());
            }
            inGame = true;
            _saberLight1->set_enabled(true);
            _saberLight2->set_enabled(true);
            UpdateLightValues();
        });

        SceneEventManager::OnMenuEnter += CP_SDK::Utils::Action<>([]
        {
            getLogger().info("hi menu");
            inGame = false;
            _saberLight1->set_enabled(false);
            _saberLight2->set_enabled(false);
            UpdateLightValues();
        });
    }

    void LightManager::Update()
    {
        if(inGame)
        {
            UpdateGameLights();
        }
        else
        {
            UpdateMenuLights();
        }
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
            _platform = UnityEngine::GameObject::New_ctor("Beatmap Lighting")->AddComponent<UnityEngine::Light*>();
            static auto setType = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, UnityEngine::LightType>("UnityEngine.Light::set_type");
            setType(_platform.ptr(), UnityEngine::LightType::Directional);

            UnityEngine::GameObject::DontDestroyOnLoad(_platform.ptr()->get_gameObject());
        }

        //Saber Light (L)
        {
            _saberLight1 = UnityEngine::GameObject::New_ctor("Saber (L) Lighting")->AddComponent<UnityEngine::Light*>();
            static auto setType = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, UnityEngine::LightType>("UnityEngine.Light::set_type");
            setType(_saberLight1.ptr(), UnityEngine::LightType::Point);

            UnityEngine::GameObject::DontDestroyOnLoad(_saberLight1.ptr()->get_gameObject());
        }

        //Saber Light (R)
        {
            _saberLight2 = UnityEngine::GameObject::New_ctor("Saber (R) Lighting")->AddComponent<UnityEngine::Light*>();
            static auto setType = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, UnityEngine::LightType>("UnityEngine.Light::set_type");
            setType(_saberLight2.ptr(), UnityEngine::LightType::Point);

            UnityEngine::GameObject::DontDestroyOnLoad(_saberLight2.ptr()->get_gameObject());
        }

        UpdateLightValues();
    }

    void LightManager::UpdateLightValues()
    {
        auto settings = Config::ConfigManager::GetLightingSettings();
        if(_light.isAlive() && _light.isHandleValid())
        {
            _light.ptr()->set_intensity(settings.globalLightIntensity);
            _light.ptr()->set_color(settings.globalColor);
            _light.ptr()->get_transform()->set_rotation(UnityEngine::Quaternion::Euler(settings.lightRotation));
        }

        if(!inGame)
        {
            if(_platform.isAlive() && _platform.isHandleValid())
            {
                _platform.ptr()->set_intensity(settings.globalLightIntensity * settings.beatmapLightingBrightness);
                _platform.ptr()->set_color(settings.globalColor);
                _platform.ptr()->get_transform()->set_rotation(UnityEngine::Quaternion::Euler(settings.lightRotation));
            }
        }
        static auto setRange = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, float>("UnityEngine.Light::set_range");

        _saberLight1.ptr()->set_intensity(settings.saberLightingIntensity);
        setRange(_saberLight1.ptr(), settings.saberLightingRange);

        _saberLight2.ptr()->set_intensity(settings.saberLightingIntensity);
        setRange(_saberLight2.ptr(), settings.saberLightingRange);
    }

    void LightManager::UpdateSaberLight(GlobalNamespace::SaberType type)
    {
        UnityEngine::Light* light;
        GlobalNamespace::Saber* saber;
        if(!_saberManager || _saberManager.ptr() == nullptr) return;
        if(!_colorManager || _colorManager.ptr() == nullptr) return;
        UnityEngine::Color color = _colorManager.ptr()->ColorForSaberType(type);
        if (type == GlobalNamespace::SaberType::SaberA)
        {
            light = _saberLight1.ptr();
            saber = _saberManager.ptr()->leftSaber;
        }
        else
        {
            light = _saberLight2.ptr();
            saber = _saberManager.ptr()->rightSaber;
        }

        light->set_enabled(saber->get_isActiveAndEnabled());
        if (light->get_enabled())
        {
            light->set_color(color);
            light->get_transform()->set_position(UnityEngine::Vector3::Lerp(saber->saberBladeTopPos, saber->saberBladeBottomPos, 0.25f));
        }
    }

    void LightManager::UpdateMenuLights()
    {
        UpdateLightValues();
    }

    void LightManager::UpdateGameLights()
    {
        auto settings = Config::ConfigManager::GetLightingSettings();
        if(settings.beatmapLighting)
        {
            getLogger().info("beatmap Lighting!!");
            UnityEngine::Color color;
            int lights = 0;
            for (auto dirLight : GlobalNamespace::DirectionalLight::get_lights()->items)
            {
                getLogger().info("light %d", lights);
                if(lights > 4) break;
                if(dirLight == nullptr) continue;
                if(!dirLight->get_isActiveAndEnabled()) continue;
                getLogger().info("x1");
                float distance = dirLight->get_transform()->get_position().get_magnitude();
                float radius = dirLight->radius;
                float intensity = dirLight->intensity;
                getLogger().info("x2");
                if (radius > 0.0f && radius > distance)
                {
                    getLogger().info("x3");
                    intensity = intensity * (radius - distance) / radius; //Intensity of light relative to the distance to player
                    UnityEngine::Color toAdd = UnityEngine::Color(dirLight->color.r * intensity,
                                                                  dirLight->color.g * intensity,
                                                                  dirLight->color.b * intensity,
                                                                  0.0f); //Multiplied color
                    color.r += toAdd.r;
                    color.g += toAdd.g;
                    color.b += toAdd.b;
                    color.a = std::max(color.a, intensity);
                    getLogger().info("x4");
                }
                lights++;
            }
            getLogger().info("Calc lights");
            auto maxComponents = { 1.0f, color.r, color.g, color.b };
            float maxComponent = *std::max_element(std::begin(maxComponents), std::end(maxComponents));
            UnityEngine::Color normalizedColor = UnityEngine::Color(color.r / maxComponent, color.g / maxComponent, color.b / maxComponent, 1.0f);
            color.a = std::min(color.a, maxComponent);
            getLogger().info("got color");
            if (settings.beatmapLightingColorIntensity < 1.0f || settings.beatmapLightingMinimumBrightness > 0.0f)
            {
                float hue;
                float sat;
                float val;
                UnityEngine::Color::RGBToHSV(normalizedColor, byref(hue), byref(sat), byref(val)); //Extract HSV values from the normalized color

                sat *= settings.beatmapLightingColorIntensity; //Saturate the color by the brightness(color intensity)
                val = std::max(val, settings.beatmapLightingMinimumBrightness); //Manually brighten the color based on minimum brightness

                normalizedColor = UnityEngine::Color::HSVToRGB(hue, sat, val); //Convert back to rgb
                color.a = std::max(color.a, settings.beatmapLightingMinimumBrightness); //Set brightness to atleast the minimum brightness
                getLogger().info("applied min brightness");
            }

            //Base Lighting
            _light->set_color(normalizedColor);
            _light->set_intensity(color.a);

            //Platform lighting
            _platform->set_color(normalizedColor);
            _platform->set_intensity(color.a * settings.beatmapLightingBrightness);
            getLogger().info("set light values");
        }
        if(settings.saberLighting)
        {
            UpdateSaberLight(GlobalNamespace::SaberType::SaberA);
            UpdateSaberLight(GlobalNamespace::SaberType::SaberB);
        }
    }
}
