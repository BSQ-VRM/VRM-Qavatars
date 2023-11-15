#pragma once

#include <utility>

#include "AvatarManager.hpp"
#include "config/config.hpp"
#include "utils/FileUtils.hpp"
namespace VRMQavatars::Config
{
    class ConfigManager
    {
    private:
        static std::string TransformVRMPath(const std::string& path)
        {
            const std::string& configPath = avaconfig_path;
            const auto name = GetFileName(path);
            auto newPath = configPath + std::string(name) + ".json";
            return newPath;
        }
        static bool InitAvatarConfig()
        {
            if(AvatarManager::currentContext != nullptr)
            {
                getAvatarConfig().Init(TransformVRMPath(AvatarManager::currentContext->fileName));
                return true;
            }
            return false;
        }
    public:
        static void SaveAvatarConfig()
        {
            if(InitAvatarConfig())
            {
                getAvatarConfig().Save();
            }
        }

        static AvatarConfig_t& GetAvatarConfig()
        {
            return getAvatarConfig();
        }

        static void SaveGlobalConfig()
        {
            getGlobalConfig().Save();
        }

        static GlobalConfig_t& GetGlobalConfig()
        {
            return getGlobalConfig();
        }

        //Offsets

        static OffsetSettings GetOffsetSettings()
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

        static void SetOffsetSettings(const OffsetSettings& settings)
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

        static FingerPoseSettings GetFingerPosingSettings()
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

        static void SetFingerPosingSettings(const FingerPoseSettings& settings)
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
    };
};