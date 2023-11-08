#include "UI/AvatarsFlowCoordinator.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "HMUI/ViewController_AnimationType.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"

DEFINE_TYPE(VRMQavatars::UI::FlowCoordinators, AvatarsFlowCoordinator);

void VRMQavatars::UI::FlowCoordinators::AvatarsFlowCoordinator::Awake() {
    getLogger().info("x");
    if (!CenterMirrorViewController || !CenterMirrorViewController->m_CachedPtr.m_value) {
        CenterMirrorViewController = QuestUI::BeatSaberUI::CreateViewController<ViewControllers::CenterMirrorViewController*>();
    }
    getLogger().info("x1");
    if (!AvatarSelectionViewController ||  !AvatarSelectionViewController->m_CachedPtr.m_value) {
        AvatarSelectionViewController = QuestUI::BeatSaberUI::CreateViewController<ViewControllers::AvatarSelectionViewController*>();
    }
    getLogger().info("x2");
    if (!AvatarSettingsViewController ||  !AvatarSettingsViewController->m_CachedPtr.m_value) {
        AvatarSettingsViewController = QuestUI::BeatSaberUI::CreateViewController<ViewControllers::AvatarSettingsViewController*>();
    }
    getLogger().info("x3");
}

void VRMQavatars::UI::FlowCoordinators::AvatarsFlowCoordinator::DidActivate(bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling) {
    if (!firstActivation) return;
    getLogger().info("x4");

    SetTitle(il2cpp_utils::newcsstr("VRM Qavatars"), HMUI::ViewController::AnimationType::In);
    showBackButton = true;
    ProvideInitialViewControllers(CenterMirrorViewController, AvatarSettingsViewController, AvatarSelectionViewController, nullptr, nullptr);
    getLogger().info("5");
}

void VRMQavatars::UI::FlowCoordinators::AvatarsFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
    this->parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
}