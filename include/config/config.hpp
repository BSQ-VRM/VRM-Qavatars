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

DECLARE_JSON_CLASS(Color2,
    VALUE_DEFAULT(float, r, {});
    VALUE_DEFAULT(float, g, {});
    VALUE_DEFAULT(float, b, {});
    VALUE_DEFAULT(float, a, {});
    Color2() = default;
    Color2(const UnityEngine::Color& from)
    {
        r = from.r;
        g = from.g;
        b = from.b;
        a = from.a;
    }
    operator UnityEngine::Color()
    {
        return UnityEngine::Color(r, g, b, a);
    }
)

DECLARE_JSON_CLASS(VectorThree,
    VALUE_DEFAULT(float, x, {});
    VALUE_DEFAULT(float, y, {});
    VALUE_DEFAULT(float, z, {});
    VectorThree() = default;
    VectorThree(const UnityEngine::Vector3& from)
    {
        x = from.x;
        y = from.y;
        z = from.z;
    }
    operator UnityEngine::Vector3()
    {
        return UnityEngine::Vector3(x, y, z);
    }
)

DECLARE_JSON_CLASS(ControllerTriggerSettings,
    VALUE_DEFAULT(std::string, A, "None");
    VALUE_DEFAULT(std::string, B, "None");
    VALUE_DEFAULT(std::string, X, "None");
    VALUE_DEFAULT(std::string, Y, "None");

    VALUE_DEFAULT(std::string, LGrip, "None");
    VALUE_DEFAULT(std::string, RGrip, "None");
    VALUE_DEFAULT(std::string, LTrigger, "None");
    VALUE_DEFAULT(std::string, RTrigger, "None");
)

DECLARE_JSON_CLASS(BlendshapeSettings,
    VALUE_DEFAULT(bool, autoBlink, false);
    VALUE_DEFAULT(float, waitTime, 4.0f);
    VALUE_DEFAULT(bool, mockEyeMovement, false);
    VALUE_DEFAULT(std::string, neutralExpression, "Neutral");
)

DECLARE_JSON_CLASS(LightingSettings,
    VALUE(Color2, globalColor);
    VALUE_DEFAULT(float, globalLightIntensity, 1.0f);
    VALUE_DEFAULT(VectorThree, lightRotation, UnityEngine::Vector3(30.0f, 240.0f, 0.0f));
    VALUE_DEFAULT(bool, beatmapLighting, true);
    VALUE_DEFAULT(float, beatmapLightingColorIntensity, 0.25f);
    VALUE_DEFAULT(float, beatmapLightingBrightness, 0.5f);
    VALUE_DEFAULT(float, beatmapLightingMinimumBrightness, 0.75f);
    VALUE_DEFAULT(bool, saberLighting, false);
    VALUE_DEFAULT(float, saberLightingIntensity, 2.0f);
    VALUE_DEFAULT(float, saberLightingRange, 1.5f);
)

DECLARE_JSON_CLASS(LocomotionSettings,
    VALUE_DEFAULT(float, footDistance, 0.1f);
    VALUE_DEFAULT(float, stepThreshold, 0.1f);
    VALUE_DEFAULT(float, stepHeight, 0.02f);
    VALUE_DEFAULT(VectorThree, stepOffset, UnityEngine::Vector3(0.0f, 0.0f, 0.0f));
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
    CONFIG_VALUE(IK, IKSettings, "IK", {});
    CONFIG_VALUE(Locomotion, LocomotionSettings, "Locomotion", {});
    CONFIG_VALUE(Lighting, LightingSettings, "Lighting", {});
    CONFIG_VALUE(ControllerTriggers, ControllerTriggerSettings, "Controller Triggers", {});
    CONFIG_VALUE(Blendshapes, BlendshapeSettings, "Blendshapes", {});

    //Global

    CONFIG_VALUE(selectedFileName, std::string, "selectedFileName", "");
    CONFIG_VALUE(hasSelected, bool, "hasSelected", false);

    CONFIG_VALUE(CalibrationType, std::string, "Calibration Type", "Match Armspans");
    CONFIG_VALUE(FixedScale, float, "Fixed Scale", 1.0f);
)

//Avatar Specific + Config Items
DECLARE_CONFIG(AvatarConfig,
    CONFIG_VALUE(Offsets, OffsetSettings, "Offsets", {});
    CONFIG_VALUE(FingerPoses, FingerPoseSettings, "Finger Poses", {});
    CONFIG_VALUE(IK, IKSettings, "IK", {});
    CONFIG_VALUE(Locomotion, LocomotionSettings, "Locomotion", {});
    CONFIG_VALUE(Lighting, LightingSettings, "Lighting", {});
    CONFIG_VALUE(ControllerTriggers, ControllerTriggerSettings, "Controller Triggers", {});
    CONFIG_VALUE(Blendshapes, BlendshapeSettings, "Blendshapes", {});

    //Individual
    CONFIG_VALUE(OverrideOffsetSettings, bool, "OverrideOffsetSettings", false);
    CONFIG_VALUE(OverrideFingerPoseSettings, bool, "OverrideFingerPoseSettings", false);
    CONFIG_VALUE(OverrideIKSettings, bool, "OverrideIKSettings", true);
    CONFIG_VALUE(OverrideLocoSettings, bool, "OverrideLocoSettings", true);
    CONFIG_VALUE(OverrideLightingSettings, bool, "OverrideLightingSettings", false);
    CONFIG_VALUE(OverrideControllerTriggerSettings, bool, "OverrideControllerTriggerSettings", true);
    CONFIG_VALUE(OverrideBlendshapeSettings, bool, "OverrideBlendshapeSettings", true);

    CONFIG_VALUE(HasAgreedToTerms, bool, "HasAgreedToTerms", false);
    CONFIG_VALUE(HasCalibrated, bool, "HasCalibrated", false);
    CONFIG_VALUE(CalibratedScale, float, "CalibratedScale", 1.0f);
)