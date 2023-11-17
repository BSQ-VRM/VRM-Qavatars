#include "microphone/Microphone.hpp"
namespace VRMQavatars {

    UnityEngine::AudioClip* Microphone::Start(StringW deviceName, bool loop, int lengthSec, int frequency) {
        int microphoneDeviceIDFromName = GetMicrophoneDeviceIDFromName(deviceName);
        if (microphoneDeviceIDFromName == -1)
            throw std::runtime_error(fmt::format("Couldn't acquire device ID for device name {}", deviceName));
        if (lengthSec <= 0)
            throw std::runtime_error(fmt::format("Length of recording must be greater than zero seconds (was: {}  seconds)", lengthSec));
        if (lengthSec > 3600)
            throw std::runtime_error(fmt::format("Length of recording must be less than one hour (was: {} seconds", lengthSec));
        if (frequency <= 0)
            throw std::runtime_error(fmt::format("Frequency of recording must be greater than zero (was: {} Hz)", frequency));
        return StartRecord(microphoneDeviceIDFromName, loop, (float)lengthSec, frequency);
    }

    void Microphone::End(StringW deviceName) {
        int microphoneDeviceIDFromName = GetMicrophoneDeviceIDFromName(deviceName);
        if (microphoneDeviceIDFromName != -1)
            EndRecord(microphoneDeviceIDFromName);
    }

    bool Microphone::IsRecording(StringW deviceName) {
        int microphoneDeviceIDFromName = GetMicrophoneDeviceIDFromName(deviceName);
		return microphoneDeviceIDFromName != -1 && IsRecording(microphoneDeviceIDFromName);
    }

    int Microphone::GetPosition(StringW deviceName) {
        int microphoneDeviceIDFromName = GetMicrophoneDeviceIDFromName(deviceName);
        return microphoneDeviceIDFromName != -1 ? GetRecordPosition(microphoneDeviceIDFromName) : 0;
    }

    void Microphone::GetDeviceCaps(StringW deviceName, int& minFreq, int& maxFreq) {
        minFreq = 0;
        maxFreq = 0;
        int microphoneDeviceIDFromName = GetMicrophoneDeviceIDFromName(deviceName);
        if (microphoneDeviceIDFromName != -1)
            GetDeviceCaps(microphoneDeviceIDFromName, minFreq, maxFreq);
    }
}
