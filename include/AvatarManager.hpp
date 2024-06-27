#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Delegate.hpp>
#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Event.hpp>

#include "main.hpp"
#include "AssetLib/structure/VRM/VRMmodelContext.hpp"

#include "customTypes/TargetManager.hpp"

#include "utils/structs/OffsetPose.hpp"

#include "customTypes/FinalIK/VRIK.hpp"
#include "customTypes/FinalIK/IKSolverVR.hpp"

namespace VRMQavatars {
    class AvatarManager
    {
        public:
        static void StartupLoad();

        static AssetLib::Structure::VRM::VRMModelContext* currentContext;
        static void SetContext(AssetLib::Structure::VRM::VRMModelContext* context);

        static void SetHandOffset(const Structs::OffsetPose& pose);

        static void UpdateVRIK();

        static void SetFingerPose(const std::string& value);

        static CP_SDK::Utils::Event<> OnLoad;

        public:
        static FinalIK::VRIK* vrik;
        static TargetManager* targetManager;
    };
};