#include "UI/AvatarSettingsViewController.hpp"
#include "assets.hpp"
#include "AvatarManager.hpp"
#include "customTypes/TargetManager.hpp"

DEFINE_TYPE(VRMQavatars::UI::ViewControllers, AvatarSettingsViewController);

void VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::DidActivate(bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling) {
    if (firstActivation)
    {
        BSML::parse_and_construct(IncludedAssets::avatarSettingsView_bsml, get_transform(), this);
    }
}

void VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::UpdatePos()
{
    auto tmanager = VRMQavatars::AvatarManager::currentContext->rootGameObject->GetComponent<VRMQavatars::TargetManager*>();
    tmanager->leftHandPosX = _leftHandPosX;
    tmanager->leftHandPosY = _leftHandPosY;
    tmanager->leftHandPosZ = _leftHandPosZ;

    tmanager->leftHandRotX = _leftHandRotX;
    tmanager->leftHandRotY = _leftHandRotY;
    tmanager->leftHandRotZ = _leftHandRotZ;
}

float VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::get_leftHandPosX() { return _leftHandPosX; }
void VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::set_leftHandPosX(float value) { _leftHandPosX = value; UpdatePos(); }

float VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::get_leftHandPosY() { return _leftHandPosY; }
void VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::set_leftHandPosY(float value) { _leftHandPosY = value; UpdatePos(); }

float VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::get_leftHandPosZ() { return _leftHandPosZ; }
void VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::set_leftHandPosZ(float value) { _leftHandPosZ = value; UpdatePos(); }

float VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::get_leftHandRotX() { return _leftHandRotX; }
void VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::set_leftHandRotX(float value) { _leftHandRotX = value; UpdatePos(); }

float VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::get_leftHandRotY() { return _leftHandRotY; }
void VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::set_leftHandRotY(float value) { _leftHandRotY = value; UpdatePos(); }

float VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::get_leftHandRotZ() { return _leftHandRotZ; }
void VRMQavatars::UI::ViewControllers::AvatarSettingsViewController::set_leftHandRotZ(float value) { _leftHandRotZ = value; UpdatePos(); }