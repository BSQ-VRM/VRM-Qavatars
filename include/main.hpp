#pragma once

#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

Configuration& getConfig();
Logger& getLogger();
#ifndef GET_FIND_METHOD
#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()
#endif

constexpr const char* vrm_path = "sdcard/ModData/com.beatgames.beatsaber/Mods/VRMQavatars/Avatars";
constexpr const char* avaconfig_path = "sdcard/ModData/com.beatgames.beatsaber/Mods/VRMQavatars/AvatarConfigs/";