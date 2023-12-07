#pragma once
#include <custom-types/shared/coroutine.hpp>
#include <GlobalNamespace/OVRPlugin.hpp>
#include <sombrero/shared/FastVector3.hpp>

namespace VRMQavatars
{
    class CalibrationHelper
    {
    public:
        //Set TPose, Apply scalings, etc.
        static void PrepareContextForCalibration();

        //Get position of OVR node
        static Sombrero::FastVector3 GetPosition(GlobalNamespace::OVRPlugin::Node node);

        //Will immediately calibrate
        static void Calibrate(std::optional<float> scale);

        //4 second calibration process to allow player to align themselves and get into tpose
        static custom_types::Helpers::Coroutine StartCalibrationProc();
    };
}
