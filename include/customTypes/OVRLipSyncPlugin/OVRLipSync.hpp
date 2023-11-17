#pragma once

#include "UnityEngine/MonoBehaviour.hpp"

#include "chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp"
#include "chatplex-sdk-bs/shared/CP_SDK/ChatPlexSDK.hpp"

extern "C"
{
    #include "OVRLipSync/OVRLipSync.h"
}

namespace VRMQavatars::OVRLipSync
{
    class OVRLipSync : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.OVRLipSync", OVRLipSync, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(OVRLipSync);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(OVRLipSync);
    public:

        DECLARE_INSTANCE_METHOD(void, Awake);
        DECLARE_INSTANCE_METHOD(void, OnDestroy);

    public:
        static ovrLipSyncResult sInitialized;
        static SafePtrUnity<OVRLipSync> sInstance;

        static ovrLipSyncResult CreateContext(ovrLipSyncContext& context, ovrLipSyncContextProvider provider, int sampleRate = 0, bool enableAcceleration = false);
        static ovrLipSyncResult DestroyContext(ovrLipSyncContext context);
        static ovrLipSyncResult Initialize();
        static ovrLipSyncResult IsInitialized();
        static ovrLipSyncResult ProcessFrame(ovrLipSyncContext context, ArrayW<float> audioBuffer, ovrLipSyncFrame frame, bool stereo = true);
        static ovrLipSyncResult SendSignal(ovrLipSyncContext context, ovrLipSyncSignals signal, int arg1, int arg2);
        static void Shutdown();
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::OVRLipSync::OVRLipSync);