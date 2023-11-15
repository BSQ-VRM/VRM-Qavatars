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

DECLARE_JSON_CLASS(OffsetSettings,
    VALUE(OffsetPoseConfig, handOffset);
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

    //Global

    CONFIG_VALUE(CalibrationType, std::string, "Calibration Type", "Match Armspans");
    CONFIG_VALUE(FixedScale, float, "Fixed Scale", 1.0f);
)

//Avatar Specific + Config Items
DECLARE_CONFIG(AvatarConfig,
    CONFIG_VALUE(Offsets, OffsetSettings, "Offsets", {});
    CONFIG_VALUE(FingerPoses, FingerPoseSettings, "Finger Poses", {});

    //Individual
    CONFIG_VALUE(OverrideOffsetSettings, bool, "OverrideOffsetSettings", false);
    CONFIG_VALUE(OverrideFingerPoseSettings, bool, "OverrideFingerPoseSettings", false);
    CONFIG_VALUE(OverrideIKSettings, bool, "OverrideIKSettings", false);
    CONFIG_VALUE(OverrideLocoSettings, bool, "OverrideLocoSettings", false);

    CONFIG_VALUE(HasAgreedToTerms, bool, "HasAgreedToTerms", false);
    CONFIG_VALUE(HasCalibrated, bool, "HasCalibrated", false);
    CONFIG_VALUE(CalibratedScale, float, "CalibratedScale", 1.0f);
)