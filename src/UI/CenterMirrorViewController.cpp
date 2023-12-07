#include "UI/CenterMirrorViewController.hpp"

#include "MirrorManager.hpp"

DEFINE_TYPE(VRMQavatars::UI::ViewControllers, CenterMirrorViewController);

void VRMQavatars::UI::ViewControllers::CenterMirrorViewController::DidActivate(bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling) {
    MirrorManager::CreateMirror({2.0f, 1.0f, 3.0f}, {0.0f, 45.0f, 0.0f}, {20.0f, 20.0f}, 1.0f, 0, 60.0f);
}