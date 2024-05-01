#include "config/ConfigManager.hpp"

#include "AvatarManager.hpp"

#include "main.hpp"
#include "utils/FileUtils.hpp"

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
            cfg.Blendshape.SetValue(cfg.Blendshape.GetDefaultValue(), save);
            cfg.Lighting.SetValue(cfg.Lighting.GetDefaultValue(), save);
            cfg.Locomotion.SetValue(cfg.Locomotion.GetDefaultValue(), save);
            cfg.Offset.SetValue(cfg.Offset.GetDefaultValue(), save);
            cfg.CalibratedScale.SetValue(cfg.CalibratedScale.GetDefaultValue(), save);
            cfg.ControllerTrigger.SetValue(cfg.ControllerTrigger.GetDefaultValue(), save);
            cfg.FingerPose.SetValue(cfg.FingerPose.GetDefaultValue(), save);
            cfg.HasCalibrated.SetValue(cfg.HasCalibrated.GetDefaultValue(), save);
            cfg.IK.SetValue(cfg.IK.GetDefaultValue(), save);
            cfg.OverrideBlendshapeSettings.SetValue(cfg.OverrideBlendshapeSettings.GetDefaultValue(), save);
            cfg.OverrideLightingSettings.SetValue(cfg.OverrideLightingSettings.GetDefaultValue(), save);
            cfg.OverrideLocomotionSettings.SetValue(cfg.OverrideLocomotionSettings.GetDefaultValue(), save);
            cfg.OverrideOffsetSettings.SetValue(cfg.OverrideOffsetSettings.GetDefaultValue(), save);
            cfg.HasAgreedToTerms.SetValue(cfg.HasAgreedToTerms.GetDefaultValue(), save);
            cfg.OverrideFingerPoseSettings.SetValue(cfg.OverrideFingerPoseSettings.GetDefaultValue(), save);
            cfg.OverrideIKSettings.SetValue(cfg.OverrideIKSettings.GetDefaultValue(), save);
        }
    }

    //Create a config file for the current avatar if one doesn't exist, if there is no avatar selected then return false
    bool ConfigManager::InitAvatarConfig()
    {
        if(AvatarManager::currentContext != nullptr)
        {
            auto path = TransformVRMPath(AvatarManager::currentContext->fileName);
            if(getAvatarConfig().__config_path != path)
            {
                VRMLogger.info("Loading Avatar Config: {}", path.c_str());
                getAvatarConfig().Init(path);
            }
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
    MANAGER_ITEM_CPP_BOTH(IK)

    //Offsets
    MANAGER_ITEM_CPP_BOTH(Offset)

    //Finger Posing
    MANAGER_ITEM_CPP_BOTH(FingerPose)

    //Lighting
    MANAGER_ITEM_CPP_BOTH(Lighting)

    //Locomotion
    MANAGER_ITEM_CPP_BOTH(Locomotion)

    //Controller Triggers
    MANAGER_ITEM_CPP_AVATAR(ControllerTrigger)

    //BlendShapes
    MANAGER_ITEM_CPP_BOTH(Blendshape)

    //VMC
    MANAGER_ITEM_CPP_GLOBAL(VMC)

    //Mirror
    MANAGER_ITEM_CPP_GLOBAL(Mirror)

    //Mirror
    MANAGER_ITEM_CPP_GLOBAL(Wind)
}