#pragma once

#include "AvatarManager.hpp"
#include "config/config.hpp"

#define MANAGER_ITEM(type) \
    static type ## Settings Get ## type ## Settings(); \
    static void Set ## type ## Settings(const type ## Settings& settings); \
    static void Reset ## type ## Settings(); \

#define MANAGER_ITEM_CPP_AVATAR(type) \
    type ## Settings ConfigManager::Get ## type ## Settings() \
    { \
        if(InitAvatarConfig()) \
        { \
            if(getAvatarConfig().Override ## type ## Settings.GetValue()) \
            { \
                return getAvatarConfig(). type .GetValue(); \
            } \
        } \
        return getGlobalConfig(). type .GetValue(); \
    } \
    void ConfigManager::Set ## type ## Settings(const type ## Settings& settings) \
    { \
        if(InitAvatarConfig()) \
        { \
            if(getAvatarConfig().Override ## type ## Settings.GetValue()) \
            { \
                getAvatarConfig(). type .SetValue(settings); \
                return; \
            } \
        } \
        getGlobalConfig(). type .SetValue(settings); \
    } \
    void ConfigManager::Reset ## type ## Settings() \
    { \
        if(InitAvatarConfig()) \
        { \
            if(getAvatarConfig().Override ## type ## Settings.GetValue()) \
            { \
                getAvatarConfig(). type .SetValue(getAvatarConfig(). type .GetDefaultValue()); \
                return; \
            } \
        } \
        getGlobalConfig(). type .SetValue(getGlobalConfig(). type .GetDefaultValue()); \
    } \

#define MANAGER_ITEM_CPP_GLOBAL(type) \
    type ## Settings ConfigManager::Get ## type ## Settings() \
    { \
        return getGlobalConfig(). type .GetValue(); \
    } \
    void ConfigManager::Set ## type ## Settings(const type ## Settings& settings) \
    { \
        getGlobalConfig(). type .SetValue(settings); \
    } \
    void ConfigManager::Reset ## type ## Settings() \
    { \
        getGlobalConfig(). type .SetValue(getGlobalConfig(). type .GetDefaultValue()); \
    } \

namespace VRMQavatars::Config
{
    class ConfigManager
    {
        static std::string TransformVRMPath(const std::string& path);

        static bool InitAvatarConfig();
    public:
        static void ResetAvatarConfig(bool save = false);

        static void SaveAvatarConfig();

        static AvatarConfig_t& GetAvatarConfig();

        static void SaveGlobalConfig();

        static GlobalConfig_t& GetGlobalConfig();

        //IK

        MANAGER_ITEM(IK)

        //Offsets

        MANAGER_ITEM(Offset)

        //Finger Posing

        MANAGER_ITEM(FingerPose)

        //Lighting

        MANAGER_ITEM(Lighting)

        //Locomotion

        MANAGER_ITEM(Locomotion)

        //Controller Triggers

        MANAGER_ITEM(ControllerTrigger)

        //BlendShapes

        MANAGER_ITEM(Blendshape)

        //VMC

        MANAGER_ITEM(VMC)

        //Mirror

        MANAGER_ITEM(Mirror)

        //Mirror

        MANAGER_ITEM(Wind)
    };
};