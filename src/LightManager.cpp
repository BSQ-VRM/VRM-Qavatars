#include "LightManager.hpp"
#include "LightManager.hpp"

#include "SceneEventManager.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Light.hpp"
#include "UnityEngine/LightShadows.hpp"
#include "UnityEngine/LightType.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "UnityEngine/WaitForSeconds.hpp"

#include "GlobalNamespace/DirectionalLight.hpp"
#include "GlobalNamespace/LightGroup.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"
#include "GlobalNamespace/LightPairRotationEventEffect.hpp"
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
    std::vector<SafePtrUnity<GlobalNamespace::TubeBloomPrePassLight>> LightManager::lights;

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(LightManager)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(LightManager)
    {

    }

    custom_types::Helpers::Coroutine LightManager::Grab()
    {
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(0.1f));
        lights.clear();

        //Find Front Lights First
        auto frontLight = UnityEngine::GameObject::Find("Environment/FrontLights");
        if (frontLight != nullptr)
        {
            //Exists
            auto frontLights = frontLight->GetComponentsInChildren<GlobalNamespace::TubeBloomPrePassLight*>();
            //add lights
            for (auto front_light : frontLights)
            {
                lights.push_back(front_light);
            }
        }
        else
        {
            //Frontlight doesn't exist

            //Find Left Laser
            auto leftLaser = UnityEngine::GameObject::Find("Environment/LeftLaser");
            if (leftLaser != nullptr)
            {
                auto leftLaserLights = leftLaser->GetComponentsInChildren<GlobalNamespace::TubeBloomPrePassLight*>();
                //add lights
                for (auto laser_light : leftLaserLights)
                {
                    lights.push_back(laser_light);
                }
            }
            //Find Right Laser
            auto rightLaser = UnityEngine::GameObject::Find("Environment/RightLaser");
            if (rightLaser != nullptr)
            {
                auto rightLaserLights = rightLaser->GetComponentsInChildren<GlobalNamespace::TubeBloomPrePassLight*>();
                //add lights
                for (auto laser_light : rightLaserLights)
                {
                    lights.push_back(laser_light);
                }
            }
        }

        //Now to add rotating lights :)

        auto rotatingLightPairs = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LightPairRotationEventEffect*>();
        for (auto pair : rotatingLightPairs)
        {
            for (auto rotating_light : pair->GetComponentsInChildren<GlobalNamespace::TubeBloomPrePassLight*>())
            {
                lights.push_back(rotating_light);
            }
        }


        //MOAR LIGHTS

        auto trackLaneRings = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::TrackLaneRing*>();
        for (auto pair : trackLaneRings)
        {
            auto name = pair->get_gameObject()->get_name();
            for (auto trackLight : pair->GetComponentsInChildren<GlobalNamespace::TubeBloomPrePassLight*>())
            {
                if(trackLight->get_transform()->get_parent()->get_name() == name)
                {
                    lights.push_back(trackLight);
                }
            }
        }

        //MOARRRRRRRRRRRRRR

        auto lightGroups = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LightGroup*>();
        for (auto group : lightGroups)
        {
            for (auto Light : group->GetComponentsInChildren<GlobalNamespace::TubeBloomPrePassLight*>())
            {
                lights.push_back(Light);
            }
        }

        //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
        auto rings = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::TrackLaneRing*>();
        for (auto ring : rings)
        {
            for (auto light : ring->GetComponentsInChildren<GlobalNamespace::TubeBloomPrePassLight*>())
            {
                lights.push_back(light);
            }
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
            CP_SDK::Unity::MTCoroutineStarter::Start(Grab());
            inGame = true;
            UpdateLightValues();
        });

        SceneEventManager::OnMenuEnter += CP_SDK::Utils::Action<>([]
        {
            inGame = false;
            UpdateLightValues();
        });
    }

    void LightManager::Update()
    {
        if(inGame)
        {
            UpdateGameLights();
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

        //Saber Light (R)
        {
            auto _saberLight2 = UnityEngine::GameObject::New_ctor("Saber (R) Lighting")->AddComponent<UnityEngine::Light*>();
            static auto setType = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, UnityEngine::LightType>("UnityEngine.Light::set_type");
            setType(_saberLight2, UnityEngine::LightType::Point);
            UnityEngine::GameObject::DontDestroyOnLoad(_saberLight2->get_gameObject());
            _saberLight2->get_transform()->set_position({0.0f, 1.5f, 0.0f});
            _saberLight2->set_color({1.0f, 1.0f, 1.0f, 1.0f});
            _saberLight2->set_intensity(100.0f);
            static auto setRange = il2cpp_utils::resolve_icall<void, UnityEngine::Light*, float>("UnityEngine.Light::set_range");
            setRange(_saberLight2, 100.0f);
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
            UnityEngine::Color color;
            int lightCount = 0;
            for (auto dirLight : GlobalNamespace::DirectionalLight::get_lights()->_items)
            {
                if(lightCount > 4) break;
                if(dirLight == nullptr) continue;
                if(!dirLight->get_isActiveAndEnabled()) continue;

                float distance = dirLight->get_transform()->get_position().get_magnitude();
                float radius = dirLight->radius;
                float intensity = dirLight->intensity;
                if (radius > 0.0f && radius > distance)
                {
                    intensity = intensity * (radius - distance) / radius; //Intensity of light relative to the distance to player
                    UnityEngine::Color toAdd = UnityEngine::Color(dirLight->color.r * intensity,
                                                                  dirLight->color.g * intensity,
                                                                  dirLight->color.b * intensity,
                                                                  0.0f); //Multiplied color
                    color.r += toAdd.r;
                    color.g += toAdd.g;
                    color.b += toAdd.b;
                    color.a = std::max(color.a, intensity);
                }
                lightCount++;
            }

            for (auto light : lights)
            {
                if(!light || !light.isHandleValid() || !light.isAlive()) continue;
                if(!light.ptr()->get_isActiveAndEnabled()) continue;
                float alpha = light.ptr()->color.a; //Multiply color by alpha of color for intensity.
                UnityEngine::Color toAdd = UnityEngine::Color(light.ptr()->color.r * alpha,
                                                              light.ptr()->color.g * alpha,
                                                              light.ptr()->color.b * alpha,
                                                                  0.0f); //Multiplied color
                color.r += toAdd.r;
                color.g += toAdd.g;
                color.b += toAdd.b;
                color.a = std::max(color.a, alpha);
            }

            auto maxComponents = { 1.0f, color.r, color.g, color.b };
            float maxComponent = *std::max_element(std::begin(maxComponents), std::end(maxComponents));
            UnityEngine::Color normalizedColor = UnityEngine::Color(color.r / maxComponent, color.g / maxComponent, color.b / maxComponent, 1.0f);
            color.a = std::min(color.a, maxComponent);
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
            }

            //Base Lighting
            _light->set_color(normalizedColor);
            _light->set_intensity(color.a);

            //Platform lighting
            _platform->set_color(normalizedColor);
            _platform->set_intensity(color.a * settings.beatmapLightingBrightness);
        }
    }
}
