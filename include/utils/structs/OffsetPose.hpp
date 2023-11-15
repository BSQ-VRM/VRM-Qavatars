#pragma once

namespace VRMQavatars::Structs
{
    struct OffsetPose
    {
        float posX;
        float posY;
        float posZ;

        float rotX;
        float rotY;
        float rotZ;
        OffsetPose() = default;
        OffsetPose(float posX, float posY, float posZ, float rotX, float rotY, float rotZ) : posX(posX), posY(posY), posZ(posZ), rotX(rotX), rotY(rotY), rotZ(rotZ) {}
    };
};