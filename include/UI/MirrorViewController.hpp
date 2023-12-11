#pragma once

#include <chatplex-sdk-bs/shared/CP_SDK/Utils/Il2cpp.hpp>
#include <chatplex-sdk-bs/shared/CP_SDK_BS/UI/ViewController.hpp>

#include "main.hpp"
#include "HMUI/ViewController.hpp"

namespace VRMQavatars::UI::ViewControllers
{
    class MirrorViewController : public CP_SDK_BS::UI::ViewController
    {
        CP_SDK_IL2CPP_INHERIT("VRMQavatars.UI.ViewControllers", MirrorViewController, CP_SDK_BS::UI::ViewController);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(MirrorViewController);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(MirrorViewController);
        SDK_UI_VIEW_CONTROLLER_INSTANCE();
        void DidActivate();
    };
}

CP_SDK_IL2CPP_INHERIT_HELPERS(VRMQavatars::UI::ViewControllers::MirrorViewController);