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
                return getAvatarConfig().IKSettings.GetValue();
            }
        }
        return getGlobalConfig().IKSettings.GetValue();
    }

    void ConfigManager::SetIKSettings(const IKSettings& settings)
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideIKSettings.GetValue())
            {
                getAvatarConfig().IKSettings.SetValue(settings);
                getAvatarConfig().Save();
                return;
            }
        }
        getGlobalConfig().IKSettings.SetValue(settings);
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
                return getAvatarConfig().LightingSettings.GetValue();
            }
        }
        return getGlobalConfig().LightingSettings.GetValue();
    }

    void ConfigManager::SetLightingSettings(const LightingSettings& settings)
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideLightingSettings.GetValue())
            {
                getAvatarConfig().LightingSettings.SetValue(settings);
                getAvatarConfig().Save();
                return;
            }
        }
        getGlobalConfig().LightingSettings.SetValue(settings);
        getGlobalConfig().Save();
    }

    //Locomotion
    LocomotionSettings ConfigManager::GetLocomotionSettings()
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideLightingSettings.GetValue())
            {
                return getAvatarConfig().LocomotionSettings.GetValue();
            }
        }
        return getGlobalConfig().LocomotionSettings.GetValue();
    }

    void ConfigManager::SetLocomotionSettings(const LocomotionSettings& settings)
    {
        if(InitAvatarConfig())
        {
            if(getAvatarConfig().OverrideLightingSettings.GetValue())
            {
                getAvatarConfig().LocomotionSettings.SetValue(settings);
                getAvatarConfig().Save();
                return;
            }
        }
        getGlobalConfig().LocomotionSettings.SetValue(settings);
        getGlobalConfig().Save();
    }
}