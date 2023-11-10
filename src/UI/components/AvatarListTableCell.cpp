#include "UI/components/AvatarListTableCell.hpp"

#include "HMUI/Touchable.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Networking/DownloadHandler.hpp"
#include "UnityEngine/Networking/DownloadHandlerTexture.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestTexture.hpp"

#include "assets.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/getters.hpp"
#include "bsml/shared/Helpers/utilities.hpp"

#include "AssetLib/mappings/gLTFImageReader.hpp"

DEFINE_TYPE(VRMQavatars::UI::Components, AvatarListTableCell);

using namespace UnityEngine;
using namespace UnityEngine::UI;

UnityEngine::Color selectedColor = UnityEngine::Color(0.8f, 0.4f, 0.8f, 0.8f);
UnityEngine::Color highlightedColor = UnityEngine::Color(0.3f, 0.3f, 0.3f, 0.7f);
UnityEngine::Color idleColor = UnityEngine::Color(0.0f, 0.0f, 0.0f, 0.4f);

namespace VRMQavatars::UI::Components
{
    VRMQavatars::UI::Components::AvatarListTableCell* AvatarListTableCell::CreateNewCell()
    {
        static ConstString playerTableCellStr("GlobalLeaderboardTableCell");
        auto cellGO = UnityEngine::GameObject::New_ctor();
        auto playerCell = cellGO->AddComponent<AvatarListTableCell*>();
        cellGO->set_name(playerTableCellStr);

        BSML::parse_and_construct(IncludedAssets::avatarCell_bsml, cellGO->get_transform(), playerCell);
        return playerCell;
    }

    void AvatarListTableCell::PostParse()
    {
        get_gameObject()->AddComponent<HMUI::Touchable*>();

        backgroundImage->background->set_color(idleColor);
        backgroundImage->background->set_color0({1.0f, 1.0f, 1.0f, 1.0f});
        backgroundImage->background->set_color1({1.0f, 1.0f, 1.0f, 0.7f});
    }

    void AvatarListTableCell::HighlightDidChange(HMUI::SelectableCell::TransitionType transitionType)
    {
        backgroundImage->background->set_color(get_highlighted() ? highlightedColor : idleColor);
    }

    void AvatarListTableCell::Select()
    {
        if (onSelect)
            onSelect(this);
    }

    void AvatarListTableCell::SetDescriptor(VRMQavatars::VRMDescriptor descriptor)
    {
        this->descriptor = descriptor;
        set_name(descriptor.name);
        set_sub(descriptor.author);

        image->set_sprite(BSML::Utilities::LoadSpriteFromTexture(descriptor.thumbnail));
    }

    void AvatarListTableCell::set_name(std::string_view nameText)
    {
        name->set_text(nameText);
    }

    void AvatarListTableCell::set_sub(std::string_view subText)
    {
        sub->set_text(subText);
    }
}