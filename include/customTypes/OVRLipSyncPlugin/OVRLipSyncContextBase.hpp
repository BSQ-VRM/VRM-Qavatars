#pragma once

#include "OVRLipSync.hpp"

#include "UnityEngine/AudioSource.hpp"

namespace VRMQavatars::OVRLipSync {
    class OVRLipSyncContextBase : public UnityEngine::MonoBehaviour
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.OVRLipSync", OVRLipSyncContextBase, UnityEngine::MonoBehaviour);
        CP_SDK_IL2CPP_DECLARE_CTOR(OVRLipSyncContextBase);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR(OVRLipSyncContextBase);
    public:

        DECLARE_INSTANCE_METHOD(void, Awake);
        DECLARE_INSTANCE_METHOD(void, OnDestroy);

        DECLARE_INSTANCE_FIELD(UnityEngine::AudioSource*, audioSource);
    public:
        void set_smoothing(int value);
        int get_smoothing();

        ovrLipSyncFrame GetCurrentPhonemeFrame();
        void SetLaughterBlend(int amount);
        void SetVisemeBlend(int viseme, int amount);

        ovrLipSyncContext context;
        bool enableAcceleration = true;
        ovrLipSyncFrame frame = ovrLipSyncFrame();
        ovrLipSyncContextProvider provider = ovrLipSyncContextProvider_Enhanced;
    private:
        int _smoothing = 0;
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::OVRLipSync::OVRLipSyncContextBase);