#pragma once

#include "scotland2/shared/modloader.h"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

constexpr auto VRMLogger = Paper::ConstLoggerContext("vrm-qavatars");
#ifndef GET_FIND_METHOD
#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::methodInfo()
#endif

constexpr const char* vrm_path = "sdcard/ModData/com.beatgames.beatsaber/Mods/VRMQavatars/Avatars";
constexpr const char* avaconfig_path = "sdcard/ModData/com.beatgames.beatsaber/Mods/VRMQavatars/AvatarConfigs/";