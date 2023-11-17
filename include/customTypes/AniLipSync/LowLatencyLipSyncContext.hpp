#pragma once

#include "customTypes/OVRLipSyncPlugin/OVRLipSyncContextBase.hpp"
#include "UnityEngine/AudioClip.hpp"

namespace VRMQavatars::AniLipSync {
    class LowLatencyLipSyncContext : public OVRLipSync::OVRLipSyncContextBase
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.AniLipSync", LowLatencyLipSyncContext, OVRLipSync::OVRLipSyncContextBase);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(LowLatencyLipSyncContext);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(LowLatencyLipSyncContext);
    public:

        DECLARE_INSTANCE_METHOD(void, OnEnable);
        DECLARE_INSTANCE_METHOD(void, OnDisable);
        DECLARE_INSTANCE_METHOD(void, Start);
        DECLARE_INSTANCE_METHOD(void, Update);

        DECLARE_INSTANCE_FIELD(UnityEngine::AudioClip*, clip);

    public:
        float GetMicVolume();
        static int GetDataLength(int bufferLength, int head, int tail);

        float gain = 1.0f;
        int samplingFrequency = 48000;
        int head = -1;
        int lengthSeconds = 1;
        ArrayW<float> microphoneBuffer;
        float* processBuffer = new float[1024];
        int procBufSize = 1024;
        std::string selectedDevice;

        const std::string defaultDevice = "(Default Device)";
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::AniLipSync::LowLatencyLipSyncContext);