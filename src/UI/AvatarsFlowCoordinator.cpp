#include "UI/AvatarsFlowCoordinator.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "HMUI/ViewController_AnimationType.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"

DEFINE_TYPE(VRMQavatars::UI::FlowCoordinators, AvatarsFlowCoordinator);

void VRMQavatars::UI::FlowCoordinators::AvatarsFlowCoordinator::Awake() {
    if (!CenterMirrorViewController || !CenterMirrorViewController->m_CachedPtr.m_value) {
        CenterMirrorViewController = QuestUI::BeatSaberUI::CreateViewController<ViewControllers::CenterMirrorViewController*>();
    }
    if (!AvatarSelectionViewController ||  !AvatarSelectionViewController->m_CachedPtr.m_value) {
        AvatarSelectionViewController = QuestUI::BeatSaberUI::CreateViewController<ViewControllers::AvatarSelectionViewController*>();
    }
    if (!AvatarSettingsViewController ||  !AvatarSettingsViewController->m_CachedPtr.m_value) {
        AvatarSettingsViewController = QuestUI::BeatSaberUI::CreateViewController<ViewControllers::AvatarSettingsViewController*>();
    }
}

void VRMQavatars::UI::FlowCoordinators::AvatarsFlowCoordinator::DidActivate(bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling) {
    if (!firstActivation) return;

    SetTitle(il2cpp_utils::newcsstr("VRM Qavatars"), HMUI::ViewController::AnimationType::In);
    showBackButton = true;
    ProvideInitialViewControllers(CenterMirrorViewController, AvatarSettingsViewController, AvatarSelectionViewController, nullptr, nullptr);
}

void VRMQavatars::UI::FlowCoordinators::AvatarsFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
    this->parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
}