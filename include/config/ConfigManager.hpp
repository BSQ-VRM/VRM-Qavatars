#pragma once

#include <utility>

#include "AvatarManager.hpp"
#include "config/config.hpp"
#include "utils/FileUtils.hpp"
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

        static IKSettings GetIKSettings();

        static void SetIKSettings(const IKSettings& settings);

        //Offsets

        static OffsetSettings GetOffsetSettings();

        static void SetOffsetSettings(const OffsetSettings& settings);

        //Finger Posing

        static FingerPoseSettings GetFingerPosingSettings();

        static void SetFingerPosingSettings(const FingerPoseSettings& settings);

        //Lighting

        static LightingSettings GetLightingSettings();

        static void SetLightingSettings(const LightingSettings& settings);

        //Locomotion

        static LocomotionSettings GetLocomotionSettings();

        static void SetLocomotionSettings(const LocomotionSettings& settings);

        //Controller Triggers

        static ControllerTriggerSettings GetControllerTriggerSettings();

        static void SetControllerTriggerSettings(const ControllerTriggerSettings& settings);

        //BlendShapes

        static BlendshapeSettings GetBlendShapeSettings();

        static void SetBlendShapeSettings(const BlendshapeSettings& settings);

        //VMC

        static VMCSettings GetVMCSettings();

        static void SetVMCSettings(const VMCSettings& settings);

        //Mirror

        static MirrorSettings GetMirrorSettings();

        static void SetMirrorSettings(const MirrorSettings& settings);
    };
};