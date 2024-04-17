#include "customTypes/OVRLipSyncPlugin/OVRLipSync.hpp"

#include "main.hpp"

#include "UnityEngine/AudioSettings.hpp"

#include "System/Array.hpp"
#include "System/Runtime/InteropServices/GCHandle.hpp"
#include "System/Runtime/InteropServices/GCHandleType.hpp"

namespace VRMQavatars::OVRLipSync
{
    CP_SDK_IL2CPP_INHERIT_INIT(OVRLipSync);

    ovrLipSyncResult OVRLipSync::sInitialized = ovrLipSyncError_Unknown;
    SafePtrUnity<OVRLipSync> OVRLipSync::sInstance = nullptr;

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(OVRLipSync)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(OVRLipSync)
    {

    }

    void OVRLipSync::Awake()
    {
        VRMLogger.info("ovr x1");
        if(!sInstance)
        {
            VRMLogger.info("ovr x2");
            sInstance = this;
            if(IsInitialized() != ovrLipSyncSuccess)
            {
                VRMLogger.info("ovr x3");
                sInitialized = Initialize();
                VRMLogger.info("ovr x4");
                if(IsInitialized() != ovrLipSyncSuccess)
                {
                    VRMLogger.info("ovr x5");
                    VRMLogger.error("OvrLipSync Awake: Failed to init Speech Rec library");
                }
                VRMLogger.info("ovr x6");
            }
            VRMLogger.info("ovr x7");
            return;
        }
        VRMLogger.error("OVRLipSync Awake: Only one instance of OVRPLipSync can exist in the scene.");
    }

    void OVRLipSync::OnDestroy()
    {
        if (sInstance.ptr() != this)
        {
            VRMLogger.error("OVRLipSync OnDestroy: This is not the correct OVRLipSync instance.");
        }
    }

    ovrLipSyncResult OVRLipSync::CreateContext(ovrLipSyncContext& context, ovrLipSyncContextProvider provider, int sampleRate, bool enableAcceleration)
    {
        if (IsInitialized() != ovrLipSyncSuccess && Initialize() != ovrLipSyncSuccess)
        {
            return ovrLipSyncError_CannotCreateContext;
        }
        return ovrLipSyncDll_CreateContextEx(&context, provider, sampleRate, enableAcceleration);
    }

    ovrLipSyncResult OVRLipSync::DestroyContext(ovrLipSyncContext context)
    {
        if (IsInitialized() != ovrLipSyncSuccess)
        {
            return ovrLipSyncError_Unknown;
        }
        return ovrLipSyncDll_DestroyContext(context);
    }

    ovrLipSyncResult OVRLipSync::Initialize()
    {
        int outputSampleRate = UnityEngine::AudioSettings::get_outputSampleRate();
        int num;
        int num2;
        static auto buildHumanoid = il2cpp_utils::resolve_icall<void, int&, int&>("UnityEngine.AudioSettings::GetDSPBufferSize");
        buildHumanoid(num, num2);
        VRMLogger.info("OvrLipSync Awake: Queried SampleRate: %d BufferSize: %d", outputSampleRate, num);
        sInitialized = ovrLipSyncDll_Initialize(1024, num);
        return sInitialized;
    }

    ovrLipSyncResult OVRLipSync::IsInitialized()
    {
        return sInitialized;
    }

    ovrLipSyncResult OVRLipSync::ProcessFrame(ovrLipSyncContext context, float* audioBuffer, int bufferSize, ovrLipSyncFrame frame, bool stereo)
    {
        if (IsInitialized() != ovrLipSyncSuccess)
        {
            return ovrLipSyncError_Unknown;
        }

        auto audioDataType = (stereo ? ovrLipSyncAudioDataType_F32_Stereo : ovrLipSyncAudioDataType_F32_Mono);
        auto num = static_cast<uint>(stereo ? (bufferSize / 2) : bufferSize);

        auto list = System::Collections::Generic::List_1<float>::New_ctor(bufferSize);
        for (int i = 0; i < bufferSize; i++)
        {
            list->set_Item(i, audioBuffer[i]);
        }
        auto gcHandle = System::Runtime::InteropServices::GCHandle::Alloc(list, System::Runtime::InteropServices::GCHandleType::Pinned);
        const auto result = ovrLipSyncDll_ProcessFrameEx(context, gcHandle.AddrOfPinnedObject(), num, audioDataType, &frame);
        gcHandle.Free();
        return result;
    }

    ovrLipSyncResult OVRLipSync::SendSignal(ovrLipSyncContext context, ovrLipSyncSignals signal, int arg1, int arg2)
    {
        if (IsInitialized() != ovrLipSyncSuccess)
        {
            return ovrLipSyncError_Unknown;
        }

        return ovrLipSyncDll_SendSignal(context, signal, arg1, arg2);
    }

    void OVRLipSync::Shutdown()
    {
        ovrLipSyncDll_Shutdown();
        sInitialized = ovrLipSyncError_Unknown;
    }

};