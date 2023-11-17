#include "microphone/Microphone.hpp"

#include "main.hpp"

namespace VRMQavatars {

    UnityEngine::AudioClip* Microphone::Start(StringW deviceName, bool loop, int lengthSec, int frequency) {
        int microphoneDeviceIDFromName = GetMicrophoneDeviceIDFromName(deviceName);
        if (microphoneDeviceIDFromName == -1)
            getLogger().error("Couldn't acquire device ID for device name %s", std::string(deviceName).c_str());
        if (lengthSec <= 0)
            getLogger().error("Length of recording must be greater than zero seconds (was: %d  seconds)", lengthSec);
        if (lengthSec > 3600)
            getLogger().error("Length of recording must be less than one hour (was: %d seconds", lengthSec);
        if (frequency <= 0)
            getLogger().error("Frequency of recording must be greater than zero (was: %d Hz)", frequency);
        return StartRecord(microphoneDeviceIDFromName, loop, static_cast<float>(lengthSec), frequency);
    }

    void Microphone::End(const StringW deviceName) {
        if (int microphoneDeviceIDFromName = GetMicrophoneDeviceIDFromName(deviceName); microphoneDeviceIDFromName != -1)
            EndRecord(microphoneDeviceIDFromName);
    }

    bool Microphone::IsRecording(const StringW deviceName) {
        const int microphoneDeviceIDFromName = GetMicrophoneDeviceIDFromName(deviceName);
		return microphoneDeviceIDFromName != -1 && IsRecording(microphoneDeviceIDFromName);
    }

    int Microphone::GetPosition(const StringW deviceName) {
        int microphoneDeviceIDFromName = GetMicrophoneDeviceIDFromName(deviceName);
        return microphoneDeviceIDFromName != -1 ? GetRecordPosition(microphoneDeviceIDFromName) : 0;
    }

    void Microphone::GetDeviceCaps(const StringW deviceName, int& minFreq, int& maxFreq) {
        minFreq = 0;
        maxFreq = 0;
        const int microphoneDeviceIDFromName = GetMicrophoneDeviceIDFromName(deviceName);
        if (microphoneDeviceIDFromName != -1)
            GetDeviceCaps(microphoneDeviceIDFromName, minFreq, maxFreq);
    }
}
