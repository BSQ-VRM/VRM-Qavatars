#include "UI/AvatarsFlowCoordinator.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "HMUI/ViewController_AnimationType.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"

DEFINE_TYPE(VRMQavatars::UI::FlowCoordinators, AvatarsFlowCoordinator);

namespace VRMQavatars::UI
{
    void FlowCoordinators::AvatarsFlowCoordinator::Awake() {
        if (!MirrorViewController || !MirrorViewController->m_CachedPtr.m_value) {
            MirrorViewController = QuestUI::BeatSaberUI::CreateViewController<ViewControllers::MirrorViewController*>();
        }
        if (!AvatarSelectionViewController ||  !AvatarSelectionViewController->m_CachedPtr.m_value) {
            AvatarSelectionViewController = QuestUI::BeatSaberUI::CreateViewController<ViewControllers::AvatarSelectionViewController*>();
        }
        if (!AvatarSettingsViewController ||  !AvatarSettingsViewController->m_CachedPtr.m_value) {
            AvatarSettingsViewController = QuestUI::BeatSaberUI::CreateViewController<ViewControllers::AvatarSettingsViewController*>();
        }
    }

    void FlowCoordinators::AvatarsFlowCoordinator::DidActivate(const bool firstActivation, bool, bool) {
        if (!firstActivation) return;
        showBackButton = true;

        SetTitle(il2cpp_utils::newcsstr("VRM Qavatars"), HMUI::ViewController::AnimationType::In);
        ProvideInitialViewControllers(AvatarSelectionViewController, AvatarSettingsViewController, MirrorViewController, nullptr, nullptr);
    }

    void FlowCoordinators::AvatarsFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
        this->parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
    }
}