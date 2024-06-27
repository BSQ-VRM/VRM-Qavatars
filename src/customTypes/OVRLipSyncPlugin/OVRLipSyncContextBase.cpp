#include "customTypes/OVRLipSyncPlugin/OVRLipSyncContextBase.hpp"

#include "main.hpp"

namespace VRMQavatars::OVRLipSync
{
    CP_SDK_IL2CPP_INHERIT_INIT(OVRLipSyncContextBase);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(OVRLipSyncContextBase)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(OVRLipSyncContextBase)
    {

    }

    void OVRLipSyncContextBase::Awake()
    {
        if (!audioSource)
        {
            audioSource = GetComponent<UnityEngine::AudioSource*>();
        }
        if (OVRLipSync::CreateContext(context, provider, 0, enableAcceleration) != ovrLipSyncSuccess)
        {
            VRMLogger.error("OVRLipSyncContextBase.Start ERROR: Could not create Phoneme context.");
        }
    }

    ovrLipSyncFrame OVRLipSyncContextBase::GetCurrentPhonemeFrame()
    {
        return frame;
    }

    void OVRLipSyncContextBase::OnDestroy()
    {
        if (OVRLipSync::DestroyContext(context) != ovrLipSyncSuccess)
        {
            VRMLogger.error("OVRLipSyncContextBase.OnDestroy ERROR: Could not delete Phoneme context.");
        }
    }

    void OVRLipSyncContextBase::SetLaughterBlend(int amount)
    {
        if (OVRLipSync::SendSignal(context, ovrLipSyncSignals_LaughterAmount, amount, 0) != ovrLipSyncSuccess)
        {
            VRMLogger.error("OVRLipSyncContextBase.SetLaughterBlend: An unexpected error occured.");
        }
    }

    void OVRLipSyncContextBase::SetVisemeBlend(int viseme, int amount)
    {
        ovrLipSyncResult result = OVRLipSync::SendSignal(context, ovrLipSyncSignals_VisemeAmount, viseme, amount);
        if (result != ovrLipSyncSuccess)
        {
            if (result == ovrLipSyncError_InvalidParam)
            {
                VRMLogger.error("OVRLipSyncContextBase.SetVisemeBlend: Viseme ID is invalid.");
                return;
            }
            VRMLogger.error("OVRLipSyncContextBase.SetVisemeBlend: An unexpected error occured.");
        }
    }

    void OVRLipSyncContextBase::set_smoothing(int value)
    {
        ovrLipSyncResult result = OVRLipSync::SendSignal(context, ovrLipSyncSignals_VisemeSmoothing, value, 0);
        if (result != ovrLipSyncSuccess)
        {
            if (result == ovrLipSyncError_InvalidParam)
            {
                VRMLogger.error("OVRLipSyncContextBase.SetSmoothing: A viseme smoothing parameter is invalid, it should be between 1 and 100!");
            }
            else
            {
                VRMLogger.error("OVRLipSyncContextBase.SetSmoothing: An unexpected error occured.");
            }
        }
        _smoothing = value;
    }

    int OVRLipSyncContextBase::get_smoothing()
    {
        return _smoothing;
    }
};