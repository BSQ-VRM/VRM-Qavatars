#include "mini-config-utils.hpp"
#include "utils/structs/OffsetPose.hpp"

DECLARE_JSON_CLASS(OffsetPoseConfig,
    VALUE(float, posX);
    VALUE(float, posY);
    VALUE(float, posZ);
    VALUE(float, rotX);
    VALUE(float, rotY);
    VALUE(float, rotZ);
    OffsetPoseConfig(const VRMQavatars::Structs::OffsetPose& other)
    {
        posX = other.posX;
        posY = other.posY;
        posZ = other.posZ;
        rotX = other.rotX;
        rotY = other.rotY;
        rotZ = other.rotZ;
    }
    OffsetPoseConfig& operator=(const VRMQavatars::Structs::OffsetPose& other)
    {
        posX = other.posX;
        posY = other.posY;
        posZ = other.posZ;
        rotX = other.rotX;
        rotY = other.rotY;
        rotZ = other.rotZ;
        return *this;
    }
    operator VRMQavatars::Structs::OffsetPose() const
    {
        return VRMQavatars::Structs::OffsetPose(posX, posY, posZ, rotX, rotY, rotZ);
    }
    OffsetPoseConfig() = default;
    OffsetPoseConfig(float posX, float posY, float posZ, float rotX, float rotY, float rotZ) : posX(posX), posY(posY), posZ(posZ), rotX(rotX), rotY(rotY), rotZ(rotZ) {}
)

DECLARE_JSON_CLASS(LightingSettings,
    VALUE_DEFAULT(UnityEngine::Color, globalColor, UnityEngine::Color(1.0f, 1.0f, 1.0f, 1.0f));
    VALUE_DEFAULT(float, globalLightIntensity, 1.0f);
    VALUE_DEFAULT(UnityEngine::Vector3, lightRotation, UnityEngine::Vector3(30.0f, 240.0f, 0.0f));
    VALUE_DEFAULT(bool, beatmapLighting, true);
    VALUE_DEFAULT(float, beatmapLightingBrightness, 0.25f);
    VALUE_DEFAULT(float, beatmapLightingMinimumBrightness, 0.75f);
    VALUE_DEFAULT(bool, saberLighting, false);
    VALUE_DEFAULT(float, saberLightingIntensity, 2.0f);
    VALUE_DEFAULT(float, saberLightingRange, 1.5f);
)

DECLARE_JSON_CLASS(LocomotionSettings,
    VALUE_DEFAULT(float, footDistance, 0.1f);
    VALUE_DEFAULT(float, stepThreshold, 0.1f);
    VALUE_DEFAULT(float, stepHeight, 0.02f);
    VALUE_DEFAULT(UnityEngine::Vector3, stepOffset, UnityEngine::Vector3(0.0f, 0.0f, 0.0f));
)

DECLARE_JSON_CLASS(IKSettings,
    VALUE_DEFAULT(float, legSwivel, 0.0f);
    VALUE_DEFAULT(float, armSwivel, 0.0f);
    VALUE_DEFAULT(float, bodyStiffness, 0.75f);
    VALUE_DEFAULT(float, shoulderRotationWeight, 1.0f);
    VALUE_DEFAULT(float, wristTwistFixAmount, 0.3f);
    VALUE_DEFAULT(float, shoulderTwistFixAmount, 0.3f);
)

DECLARE_JSON_CLASS(OffsetSettings,
    VALUE_DEFAULT(OffsetPoseConfig, handOffset, OffsetPoseConfig(-0.02f, 0.04f, -0.13f, 55.0f, 0.0f, 90.0f));
)

DECLARE_JSON_CLASS(FingerPoseSettings,
    VALUE_DEFAULT(std::string, grabPose, "-34,-66,-80,0,-34,-66,-65,0,-37,-73,-43,0,-46,-48,-30,0,-77,-47,-20,19");
)

//Access Global through ConfigManager::GetGlobal
//Access Avatar through ConfigManager::GetAvatar
//(MANDATORY) Access individual Config items through ConfigManager::GetXXX

//Global Specific + Config Items
DECLARE_CONFIG(GlobalConfig,
    CONFIG_VALUE(Offsets, OffsetSettings, "Offsets", {});
    CONFIG_VALUE(FingerPoses, FingerPoseSettings, "Finger Poses", {});
    CONFIG_VALUE(IKSettings, IKSettings, "IK", {});
    CONFIG_VALUE(LocomotionSettings, LocomotionSettings, "Locomotion", {});
    CONFIG_VALUE(LightingSettings, LightingSettings, "Lighting", {});

    //Global

    CONFIG_VALUE(CalibrationType, std::string, "Calibration Type", "Match Armspans");
    CONFIG_VALUE(FixedScale, float, "Fixed Scale", 1.0f);
)

//Avatar Specific + Config Items
DECLARE_CONFIG(AvatarConfig,
    CONFIG_VALUE(Offsets, OffsetSettings, "Offsets", {});
    CONFIG_VALUE(FingerPoses, FingerPoseSettings, "Finger Poses", {});
    CONFIG_VALUE(IKSettings, IKSettings, "IK", {});
    CONFIG_VALUE(LocomotionSettings, LocomotionSettings, "Locomotion", {});
    CONFIG_VALUE(LightingSettings, LightingSettings, "Lighting", {});

    //Individual
    CONFIG_VALUE(OverrideOffsetSettings, bool, "OverrideOffsetSettings", false);
    CONFIG_VALUE(OverrideFingerPoseSettings, bool, "OverrideFingerPoseSettings", false);
    CONFIG_VALUE(OverrideIKSettings, bool, "OverrideIKSettings", false);
    CONFIG_VALUE(OverrideLocoSettings, bool, "OverrideLocoSettings", false);
    CONFIG_VALUE(OverrideLightingSettings, bool, "OverrideLightingSettings", false);

    CONFIG_VALUE(HasAgreedToTerms, bool, "HasAgreedToTerms", false);
    CONFIG_VALUE(HasCalibrated, bool, "HasCalibrated", false);
    CONFIG_VALUE(CalibratedScale, float, "CalibratedScale", 1.0f);
)