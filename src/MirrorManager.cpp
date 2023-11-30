#include "MirrorManager.hpp"

#include <bsml/shared/BSML/FloatingScreen/FloatingScreen.hpp>
#include <HMUI/ImageView.hpp>

#include <GlobalNamespace/MainCamera.hpp>
#include <GlobalNamespace/VisualEffectsController.hpp>

#include <UnityEngine/Shader.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/RenderTexture.hpp>
#include <UnityEngine/Sprite.hpp>
#include <UnityEngine/Vector2.hpp>
#include <UnityEngine/StereoTargetEyeMask.hpp>
#include <UnityEngine/AudioListener.hpp>
#include <UnityEngine/PrimitiveType.hpp>

#include "config/ConfigManager.hpp"
#include "customTypes/Mirror.hpp"

namespace VRMQavatars
{
    SafePtrUnity<UnityEngine::GameObject> MirrorManager::mainMirror;
    SafePtrUnity<UnityEngine::Camera> MirrorManager::mirrorCamera;
    SafePtrUnity<UnityEngine::MeshRenderer> MirrorManager::mirrorRenderer;
    SafePtrUnity<UnityEngine::Shader> MirrorManager::mirrorShader;

    UnityEngine::Sprite* GetBGSprite(std::string str)
    {
        return UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Sprite*>().First( [str](UnityEngine::Sprite* x) { return x->get_name() == str; });
    }

    UnityEngine::Material* GetBGMat(std::string str)
    {
        return UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>().First([str](UnityEngine::Material* x) { return x->get_name() == str; });
    }

    int firstPersonAvatar = 1 << 6;
    int thirdPersonAvatar = 1 << 3;
    int UI = 1 << 5;
    int Saber = 1 << 12;
    int Notes = 1 << 8;
    int Walls = 1 << 11;

    int GetMask(int layer)
    {

        constexpr int Nothing = 0;
        constexpr int Everything = 2147483647;

        getLogger().info("mask 1");
        int tpmask = 0;

        //0 All
        //1 All Except UI
        //2 Only Avatar
        //3 Avatar and Saber
        //4 Avatar, Saber, and Notes
        //5 Avatar, Saber, Notes, and Walls
        if(layer == 0)
        {
            tpmask = Everything & ~firstPersonAvatar;
        }

        if(layer == 1)
        {
            tpmask = Everything & ~UI & ~firstPersonAvatar; //UI
        }

        if(layer == 2)
        {
            tpmask = Nothing | thirdPersonAvatar; //Only Avatar
        }

        if(layer == 3)
        {
            tpmask = Nothing | thirdPersonAvatar | Saber; //Avatar and Saber
        }

        if(layer == 4)
        {
            tpmask = Nothing | thirdPersonAvatar | Saber | Notes; //Avatar, Saber, and Notes
        }

        if(layer == 5)
        {
            tpmask = Nothing | thirdPersonAvatar | Saber | Notes | Walls; //Avatar, Saber, Notes, and Walls
        }

        return tpmask;
    }

    UnityEngine::GameObject* MirrorManager::CreateMirror(const UnityEngine::Vector3 position, const UnityEngine::Vector3 rotation, const UnityEngine::Vector2 size, const int layer, float fov)
    {
        int mask = GetMask(layer);

        //Main Camera

        const auto mainCamera = UnityEngine::GameObject::FindGameObjectWithTag("MainCamera");
        //Root

        UnityEngine::GameObject* rootObject = UnityEngine::GameObject::New_ctor("Mirror");

        //Screen

        const auto screen = BSML::FloatingScreen::CreateFloatingScreen(UnityEngine::Vector2(size) * 1.05f, true, position, UnityEngine::Quaternion::Euler(rotation), 0.0f, true);
        UnityEngine::GameObject::DontDestroyOnLoad(screen->get_gameObject());

        rootObject->get_transform()->SetParent(screen->get_transform(), false);
        rootObject->get_transform()->set_localPosition({0.0f, 0.0f, 0.05f});

        getLogger().info("m3");

        //Make render texture

        const float aspect = size.x / size.y;

        const auto renderTex = UnityEngine::RenderTexture::New_ctor(aspect * 1080, 1080, 24, UnityEngine::RenderTextureFormat::_get_ARGB32());

        //Add Camera

        const auto newCamera = UnityEngine::GameObject::Instantiate(mainCamera, rootObject->get_transform(), false);
        const auto camcomp = newCamera->GetComponent<UnityEngine::Camera*>();

        getLogger().info("m4");

        UnityEngine::GameObject::DestroyImmediate(newCamera->GetComponent<UnityEngine::AudioListener*>());
        UnityEngine::GameObject::DestroyImmediate(newCamera->GetComponent<GlobalNamespace::MainCamera*>());
        UnityEngine::GameObject::DestroyImmediate(newCamera->GetComponent<GlobalNamespace::VisualEffectsController*>());

        camcomp->set_nearClipPlane(1.0f);

        getLogger().info("m5");

        camcomp->set_targetDisplay(0);
        camcomp->set_stereoTargetEye(UnityEngine::StereoTargetEyeMask::None);
        camcomp->set_tag("Untagged");
        camcomp->set_fieldOfView(fov);

        camcomp->set_targetTexture(renderTex);
        getLogger().info("m6");

        camcomp->set_cullingMask(mask);

        newCamera->get_transform()->set_localPosition({0.0f, 0.0f, -1.0f});
        newCamera->get_transform()->set_localRotation(UnityEngine::Quaternion::Euler(0.0f, 0.0f, 0.0f));

        //Quad

        const auto quad = UnityEngine::GameObject::CreatePrimitive(UnityEngine::PrimitiveType::Quad);
        quad->get_transform()->SetParent(rootObject->get_transform(), false);
        quad->get_transform()->set_localScale({-size.x, size.y, 1.0f});
        quad->get_transform()->set_localEulerAngles({0, 180, 0});

        getLogger().info("m9");

        const auto material = UnityEngine::Material::New_ctor(mirrorShader.ptr());
        material->set_mainTexture(renderTex);
        quad->GetComponent<UnityEngine::MeshRenderer*>()->set_material(material);

        //Border

        const auto getBgSprite = GetBGSprite("RoundRect10BorderFade");

        for(const auto x : screen->GetComponentsInChildren<HMUI::ImageView*>()) {
            if(!x)
                continue;
            x->skew = 0.0f;
            x->set_overrideSprite(nullptr);
            x->set_sprite(getBgSprite);
            x->set_material(GetBGMat("UINoGlow"));
            x->set_color({1.0f, 1.0f, 1.0f, 1.0f});
        }

        rootObject->AddComponent<Mirror*>();

        return rootObject;
    }

    void MirrorManager::CreateMainMirror()
    {
        auto conf = Config::ConfigManager::GetMirrorSettings();

        const auto obj = CreateMirror(conf.position, conf.rotation, conf.size, conf.layer, conf.fov);
        mainMirror = obj;
        mirrorCamera = obj->GetComponentInChildren<UnityEngine::Camera*>();
        mirrorRenderer = obj->GetComponentInChildren<UnityEngine::MeshRenderer*>();
        mainMirror->get_transform()->get_parent()->get_gameObject()->set_active(conf.enabled);

        const auto screen = mainMirror->get_transform()->get_parent()->get_gameObject();
        const auto screenComp = screen->GetComponent<BSML::FloatingScreen*>();
        screenComp->HandleReleased += [](BSML::FloatingScreen* screen, const BSML::FloatingScreenHandleEventArgs& args)
        {
            auto rot = args.rotation;
            auto settings = Config::ConfigManager::GetMirrorSettings();
            settings.position = args.position;
            settings.rotation = rot.get_eulerAngles();
            Config::ConfigManager::SetMirrorSettings(settings);
        };
    }

    void MirrorManager::UpdateMirror()
    {
        auto conf = Config::ConfigManager::GetMirrorSettings();
        const auto screen = mainMirror->get_transform()->get_parent()->get_gameObject();
        screen->set_active(conf.enabled);

        const auto screenComp = screen->GetComponent<BSML::FloatingScreen*>();
        screenComp->set_ScreenSize(UnityEngine::Vector2(conf.size) * 1.05f);
        screenComp->set_ScreenPosition(conf.position);
        screenComp->set_ScreenRotation(UnityEngine::Quaternion::Euler(conf.rotation));

        mirrorRenderer->get_transform()->set_localScale({-conf.size.x, conf.size.y, 1.0f});
        mirrorRenderer->get_transform()->set_localEulerAngles({0, 180, 0});

        const float aspect = conf.size.x / conf.size.y;
        const auto renderTex = UnityEngine::RenderTexture::New_ctor(aspect * 1080, 1080, 24, UnityEngine::RenderTextureFormat::_get_ARGB32());
        mirrorCamera->set_targetTexture(renderTex);
        mirrorRenderer->get_material()->set_mainTexture(renderTex);

        const int mask = GetMask(conf.layer);
        mirrorCamera->set_cullingMask(mask);

        mirrorCamera->set_fieldOfView(conf.fov);

        mainMirror->GetComponent<Mirror*>()->Update();
    }
}
