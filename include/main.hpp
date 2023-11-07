#pragma once

#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

Configuration& getConfig();
Logger& getLogger();

#define MAKE_HOOK_OFFSETLESS(name, retval, ...) \
retval (*name)(__VA_ARGS__) = NULL; \
retval hook_ ## name(__VA_ARGS__)

#define INSTALL_HOOK_OFFSETLESS(name, methodInfo) MACRO_WRAP( \
Logger::get().info("Installing 64 bit offsetless hook: %s at %lX", #name, asOffset(methodInfo->methodPointer)); \
A64HookFunction((void*)methodInfo->methodPointer,(void*) hook_ ## name, (void**)&name); \
)