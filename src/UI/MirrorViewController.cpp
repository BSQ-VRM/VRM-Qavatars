#include "UI\MirrorViewController.hpp"

#include "MirrorManager.hpp"

namespace VRMQavatars::UI::ViewControllers
{
    CP_SDK_IL2CPP_INHERIT_INIT(MirrorViewController);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(MirrorViewController)
    {
        OnViewCreation      = {this, &MirrorViewController::DidActivate};
    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(MirrorViewController)
    {

    }

    void MirrorViewController::DidActivate() {
        //const auto mirror = MirrorManager::CreateMirror({2.75f, 1.0f, 2.0f}, {0.0f, 225.0f, 0.0f}, {142.0f, 80.0f}, 1.77f, 3, 35.0f, {1.0f, 1.0f, 1.0f, 0.0f}, false, false, false);
        //mirror->get_transform()->get_parent()->SetParent(get_transform());
    }
}