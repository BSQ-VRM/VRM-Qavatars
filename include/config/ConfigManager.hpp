#pragma once

#include "sombrero/shared/FastVector3.hpp"
#include "config/config.hpp"

#define MANAGER_ITEM(type) \
    static type ## Settings Get ## type ## Settings(); \
    static void Set ## type ## Settings(const type ## Settings& settings); \
    static void Reset ## type ## Settings(); \

#define MANAGER_ITEM_OVERRIDE(type) \
    static bool Get ## type ## Override() \
    { \
        return InitAvatarConfig() ? getAvatarConfig().Override ## type ## Settings.GetValue() : false; \
    } \

#define MANAGER_ITEM_CPP_BOTH(type) \
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

#define MANAGER_ITEM_CPP_AVATAR(type) \
    type ## Settings ConfigManager::Get ## type ## Settings() \
    { \
        if(InitAvatarConfig()) \
        { \
            return getAvatarConfig(). type .GetValue(); \
        } \
        return type ## Settings(); \
    } \
    void ConfigManager::Set ## type ## Settings(const type ## Settings& settings) \
    { \
        if(InitAvatarConfig()) \
        { \
            getAvatarConfig(). type .SetValue(settings); \
        } \
    } \
    void ConfigManager::Reset ## type ## Settings() \
    { \
        if(InitAvatarConfig()) \
        { \
            getAvatarConfig(). type .SetValue(getAvatarConfig(). type .GetDefaultValue()); \
        } \
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
        MANAGER_ITEM_OVERRIDE(IK)

        //Offsets

        MANAGER_ITEM(Offset)
        MANAGER_ITEM_OVERRIDE(Offset)

        //Finger Posing

        MANAGER_ITEM(FingerPose)
        MANAGER_ITEM_OVERRIDE(FingerPose)

        //Lighting

        MANAGER_ITEM(Lighting)
        MANAGER_ITEM_OVERRIDE(Lighting)

        //Locomotion

        MANAGER_ITEM(Locomotion)
        MANAGER_ITEM_OVERRIDE(Locomotion)

        //Controller Triggers

        MANAGER_ITEM(ControllerTrigger)

        //BlendShapes

        MANAGER_ITEM(Blendshape)
        MANAGER_ITEM_OVERRIDE(Blendshape)

        //VMC

        MANAGER_ITEM(VMC)

        //Mirror

        MANAGER_ITEM(Mirror)

        //Mirror

        MANAGER_ITEM(Wind)
    };
};