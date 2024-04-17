#include "customTypes/AniLipSync/LowLatencyLipSyncContext.hpp"

#include "main.hpp"
#include "customTypes/OVRLipSyncPlugin/OVRLipSync.hpp"
#include "microphone/Microphone.hpp"

namespace VRMQavatars::AniLipSync
{
    CP_SDK_IL2CPP_INHERIT_INIT(LowLatencyLipSyncContext);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(LowLatencyLipSyncContext)
    {

    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(LowLatencyLipSyncContext)
    {

    }

    void LowLatencyLipSyncContext::Start()
    {
        if (Microphone::get_devices().size() == 0)
        {
            VRMLogger.error("No mic device");
            return;
        }
        microphoneBuffer = ArrayW<float>(samplingFrequency);
    }

    void LowLatencyLipSyncContext::Update()
    {
        if (clip == nullptr)
        {
            return;
        }
        const int position = Microphone::GetPosition(selectedDevice);
        if (position < 0 || head == position)
        {
            return;
        }
        clip->GetData(microphoneBuffer, 0);
        while (GetDataLength(microphoneBuffer.size(), head, position) > procBufSize)
        {
            if (const int num = microphoneBuffer.size() - head; num < procBufSize)
            {
                for (int i = 0; i < num; i++)
                {
                    processBuffer[i] = microphoneBuffer[head + i] * gain;
                }
                for (int j = num; j < procBufSize - num; j++)
                {
                    processBuffer[j] = microphoneBuffer[j - num] * gain;
                }
            }
            else
            {
                for (int k = 0; k < procBufSize; k++)
                {
                    processBuffer[k] = microphoneBuffer[head + k] * gain;
                }
            }

            OVRLipSync::OVRLipSync::ProcessFrame(context, processBuffer, procBufSize, frame, false);
            head += procBufSize;
            if (head > microphoneBuffer.size())
            {
                head -= microphoneBuffer.size();
            }
        }
    }

    void LowLatencyLipSyncContext::OnDisable()
    {
        Microphone::End(selectedDevice);
        clip = nullptr;
    }

    void LowLatencyLipSyncContext::OnEnable()
    {
        if (OVRLipSync::OVRLipSync::IsInitialized() != ovrLipSyncSuccess)
        {
            VRMLogger.error("OVRLipSync is not initialized");
            set_enabled(false);
            return;
        }
        clip = Microphone::Start(selectedDevice, true, 1, samplingFrequency);
    }

    float LowLatencyLipSyncContext::GetMicVolume()
    {
        float num = 0.0f;
        for (int k = 0; k < procBufSize; k++)
        {
            float num2 = processBuffer[k];
            num += std::powf(num2, 2.0f);
        }
        return std::sqrtf(num / static_cast<float>(procBufSize));
    }

    int LowLatencyLipSyncContext::GetDataLength(const int bufferLength, const int head, const int tail)
    {
        if (head < tail)
        {
            return tail - head;
        }
        return bufferLength - head + tail;
    }
}