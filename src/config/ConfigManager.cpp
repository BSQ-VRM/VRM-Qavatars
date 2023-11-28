#include "config/ConfigManager.hpp"

namespace VRMQavatars::Config
{
    std::string ConfigManager::TransformVRMPath(const std::string& path)
    {
        const std::string& configPath = avaconfig_path;
        const auto name = GetFileName(path);
        auto newPath = configPath + std::string(name) + ".json";
        return newPath;
    }

    void ConfigManager::ResetAvatarConfig(const bool save)
    {
        if(InitAvatarConfig())
        {
            auto& cfg = getAvatarConfig();
            cfg.Blendshapes.SetValue(cfg.Blendshapes.GetDefaultValue(), save);
            cfg.Lighting.SetValue(cfg.Lighting.GetDefaultValue(), save);
            cfg.Locomotion.SetValue(cfg.Locomotion.GetDefaultValue(), save);
            cfg.Offsets.SetValue(cfg.Offsets.GetDefaultValue(), save);
            cfg.CalibratedScale.SetValue(cfg.CalibratedScale.GetDefaultValue(), save);
            cfg.ControllerTriggers.SetValue(cfg.ControllerTriggers.GetDefaultValue(), save);
            cfg.FingerPoses.SetValue(cfg.FingerPoses.GetDefaultValue(), save);
            cfg.HasCalibrated.SetValue(cfg.HasCalibrated.GetDefaultValue(), save);
            cfg.IK.SetValue(cfg.IK.GetDefaultValue(), save);
            cfg.OverrideBlendshapeSettings.SetValue(cfg.OverrideBlendshapeSettings.GetDefaultValue(), save);
            cfg.OverrideLightingSettings.SetValue(cfg.OverrideLightingSettings.GetDefaultValue(), save);
            cfg.OverrideLocoSettings.SetValue(cfg.OverrideLocoSettings.GetDefaultValue(), save);
            cfg.OverrideOffsetSettings.SetValue(cfg.OverrideOffsetSettings.GetDefaultValue(), save);
            cfg.HasAgreedToTerms.SetValue(cfg.HasAgreedToTerms.GetDefaultValue(), save);
            cfg.OverrideControllerTriggerSettings.SetValue(cfg.OverrideControllerTriggerSettings.GetDefaultValue(), save);
            cfg.OverrideFingerPoseSettings.SetValue(cfg.OverrideFingerPoseSettings.GetDefaultValue(), save);
            cfg.OverrideIKSettings.SetValue(cfg.OverrideIKSettings.GetDefaultValue(), save);
        }
    }

    //Create a config file for the current avatar if one doesn't exist, if there is no avatar selected then return false
    bool ConfigManager::InitAvatarConfig()
    {
        if(AvatarManager::currentContext != nullptr)
        {
            getAvatarConfig().Init(TransformVRMPath(AvatarManager::currentContext->fileName));
            return true;
        }
        return false;
    }

    //Avatar Config
    void ConfigManager::SaveAvatarConfig()
    {
        if(InitAvatarConfig())
        {
            getAvatarConfig().Save();
        }
    }

    AvatarConfig_t& ConfigManager::GetAvatarConfig()
    {
        InitAvatarConfig();
        return getAvatarConfig();
    }

    //Global Config
    void ConfigManager::SaveGlobalConfig()
    {
        getGlobalConfig().Save();
    }

    GlobalConfig_t& ConfigManager::GetGlobalConfig()
    {
        return getGlobalConfig();
    }

    //Start getters/setters here

    //IK
    IKSettings ConfigManager::GetIKSettings()
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideIKSettings.GetValue())
            {
                return getAvatarConfig().IK.GetValue();
            }
        }
        return getGlobalConfig().IK.GetValue();
    }

    void ConfigManager::SetIKSettings(const IKSettings& settings)
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideIKSettings.GetValue())
            {
                getAvatarConfig().IK.SetValue(settings);
                getAvatarConfig().Save();
                return;
            }
        }
        getGlobalConfig().IK.SetValue(settings);
        getGlobalConfig().Save();
    }

    //Offsets
    OffsetSettings ConfigManager::GetOffsetSettings()
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideOffsetSettings.GetValue())
            {
                return getAvatarConfig().Offsets.GetValue();
            }
        }
        return getGlobalConfig().Offsets.GetValue();
    }

    void ConfigManager::SetOffsetSettings(const OffsetSettings& settings)
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideOffsetSettings.GetValue())
            {
                getAvatarConfig().Offsets.SetValue(settings);
                getAvatarConfig().Save();
                return;
            }
        }
        getGlobalConfig().Offsets.SetValue(settings);
        getGlobalConfig().Save();
    }

    //Finger Posing
    FingerPoseSettings ConfigManager::GetFingerPosingSettings()
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideFingerPoseSettings.GetValue())
            {
                return getAvatarConfig().FingerPoses.GetValue();
            }
        }
        return getGlobalConfig().FingerPoses.GetValue();
    }

    void ConfigManager::SetFingerPosingSettings(const FingerPoseSettings& settings)
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideFingerPoseSettings.GetValue())
            {
                getAvatarConfig().FingerPoses.SetValue(settings);
                getAvatarConfig().Save();
                return;
            }
        }
        getGlobalConfig().FingerPoses.SetValue(settings);
        getGlobalConfig().Save();
    }

    //Lighting
    LightingSettings ConfigManager::GetLightingSettings()
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideLightingSettings.GetValue())
            {
                return getAvatarConfig().Lighting.GetValue();
            }
        }
        return getGlobalConfig().Lighting.GetValue();
    }

    void ConfigManager::SetLightingSettings(const LightingSettings& settings)
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideLightingSettings.GetValue())
            {
                getAvatarConfig().Lighting.SetValue(settings);
                getAvatarConfig().Save();
                return;
            }
        }
        getGlobalConfig().Lighting.SetValue(settings);
        getGlobalConfig().Save();
    }

    //Locomotion
    LocomotionSettings ConfigManager::GetLocomotionSettings()
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideLightingSettings.GetValue())
            {
                return getAvatarConfig().Locomotion.GetValue();
            }
        }
        return getGlobalConfig().Locomotion.GetValue();
    }

    void ConfigManager::SetLocomotionSettings(const LocomotionSettings& settings)
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideLightingSettings.GetValue())
            {
                getAvatarConfig().Locomotion.SetValue(settings);
                getAvatarConfig().Save();
                return;
            }
        }
        getGlobalConfig().Locomotion.SetValue(settings);
        getGlobalConfig().Save();
    }

    //Controller Triggers
    ControllerTriggerSettings ConfigManager::GetControllerTriggerSettings()
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideControllerTriggerSettings.GetValue())
            {
                return getAvatarConfig().ControllerTriggers.GetValue();
            }
        }
        return getGlobalConfig().ControllerTriggers.GetValue();
    }

    void ConfigManager::SetControllerTriggerSettings(const ControllerTriggerSettings& settings)
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideControllerTriggerSettings.GetValue())
            {
                getAvatarConfig().ControllerTriggers.SetValue(settings);
                getAvatarConfig().Save();
                return;
            }
        }
        getGlobalConfig().ControllerTriggers.SetValue(settings);
        getGlobalConfig().Save();
    }

    //BlendShapes
    BlendshapeSettings ConfigManager::GetBlendShapeSettings()
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideBlendshapeSettings.GetValue())
            {
                return getAvatarConfig().Blendshapes.GetValue();
            }
        }
        return getGlobalConfig().Blendshapes.GetValue();
    }

    void ConfigManager::SetBlendShapeSettings(const BlendshapeSettings& settings)
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideBlendshapeSettings.GetValue())
            {
                getAvatarConfig().Blendshapes.SetValue(settings);
                getAvatarConfig().Save();
                return;
            }
        }
        getGlobalConfig().Blendshapes.SetValue(settings);
        getGlobalConfig().Save();
    }

    //VMC
    VMCSettings ConfigManager::GetVMCSettings()
    {
        return getGlobalConfig().VMC.GetValue();
    }

    void ConfigManager::SetVMCSettings(const VMCSettings& settings)
    {
        getGlobalConfig().VMC.SetValue(settings);
        getGlobalConfig().Save();
    }

    //Mirror
    static MirrorSettings GetMirrorSettings()
    {
        return getGlobalConfig().Mirror.GetValue();
    }

    static void SetMirrorSettings(const MirrorSettings& settings)
    {
        getGlobalConfig().Mirror.SetValue(settings);
        getGlobalConfig().Save();
    }
}