#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Delegate.hpp>
#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Event.hpp>

#include "customTypes/WristTwistFix.hpp"

#include "main.hpp"
#include "AssetLib/structure/VRM/VRMmodelContext.hpp"

#include "customTypes/TargetManager.hpp"

#include "utils/structs/OffsetPose.hpp"

#include "RootMotion/FinalIK/VRIK.hpp"
#include "RootMotion/FinalIK/IKSolverVR.hpp"
#include "RootMotion/FinalIK/IKSolverVR_Leg.hpp"

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

        private:
        static WristTwistFix* _wristTwistFix;

        public:
        static RootMotion::FinalIK::VRIK* vrik;
        static TargetManager* targetManager;
    };
};