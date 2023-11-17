#pragma once

#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "fmt/format.h"

namespace VRMQavatars {
    class Microphone {
        public:
            static int GetMicrophoneDeviceIDFromName(StringW name) {
                using GetMicrophoneDeviceIDFromName_fun = function_ptr_t<int, StringW>;
                static auto getMicrophoneDeviceIDFromName = reinterpret_cast<GetMicrophoneDeviceIDFromName_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::GetMicrophoneDeviceIDFromName"));
                return getMicrophoneDeviceIDFromName(name);
            }

            static UnityEngine::AudioClip* StartRecord(int deviceID, bool loop, float lengthSec, int frequency) {
                using StartRecord_fun = function_ptr_t<UnityEngine::AudioClip*, int, bool, float, int>;
                static auto startRecord = reinterpret_cast<StartRecord_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::StartRecord"));
                return startRecord(deviceID, loop, lengthSec, frequency);
            }

            static void EndRecord(int deviceID) {
                using EndRecord_fun = function_ptr_t<void, int>;
                static auto endRecord = reinterpret_cast<EndRecord_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::EndRecord"));
                endRecord(deviceID);
            }

            static bool IsRecording(int deviceID) {
                using IsRecording_fun = function_ptr_t<bool, int>;
                static auto isRecording = reinterpret_cast<IsRecording_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::IsRecording"));
                return isRecording(deviceID);
            }

            static int GetRecordPosition(int deviceID) {
                using GetRecordPosition_fun = function_ptr_t<int, int>;
                static auto getRecordPosition = reinterpret_cast<GetRecordPosition_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::GetRecordPosition"));
                return getRecordPosition(deviceID);
            }

            static void GetDeviceCaps(int deviceID, int& minFreq, int& maxFreq) {
                using GetDeviceCaps_fun = function_ptr_t<void, int, int*, int*>;
                static auto getDeviceCaps = reinterpret_cast<GetDeviceCaps_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::GetDeviceCaps"));
                getDeviceCaps(deviceID, &minFreq, &maxFreq);
            }

            static ArrayW<StringW> get_devices() {
                using GetDevices_fun = function_ptr_t<ArrayW<StringW>>;
                static auto getDevices = reinterpret_cast<GetDevices_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::get_devices"));
                return getDevices();
            }

            static bool HasUserAuthorizedPerrmission(StringW permission) {
                using UserGrantedPermssion_fun = function_ptr_t<bool, StringW>;
                static auto hasUserAuthorizedPerrmission = reinterpret_cast<UserGrantedPermssion_fun>(il2cpp_functions::resolve_icall("UnityEngine.Android.Permission::HasUserAuthorizedPermission"));
                return hasUserAuthorizedPerrmission(permission);
            }

            static UnityEngine::AudioClip* Start(StringW deviceName, bool loop, int lengthSec, int frequency);
            static void End(StringW deviceName);
            static bool IsRecording(StringW deviceName);
            static int GetPosition(StringW deviceName);
            static void GetDeviceCaps(StringW deviceName, int& minFreq, int& maxFreq);
    };
}
