#include "UI/components/AvatarListItem.hpp"

#include "bsml/shared/Helpers/utilities.hpp"

#include "assets.hpp"
#include "UI/components/AvatarListCell.hpp"

#include "UnityEngine/SpriteMeshType.hpp"

namespace VRMQavatars::UI::Components {

    AvatarListItem::AvatarListItem(const VRMDescriptor& descriptor)
    {
        this->descriptor = descriptor;
    }

    AvatarListItem::~AvatarListItem()
    {

    }

    void AvatarListItem::OnShow()
    {
        static auto cdsprite = BSML::Utilities::LoadSpriteRaw(IncludedAssets::cd_png);
        const auto l_TextListCell = GetCellAsClassOf<AvatarListCell*>();
        if (!l_TextListCell)
            return;

        l_TextListCell->NameText->Element()->TMProUGUI()->set_text(descriptor.name);
        l_TextListCell->AuthorText->Element()->TMProUGUI()->set_text(descriptor.author);
        l_TextListCell->FilenameText->Element()->TMProUGUI()->set_text(descriptor.filePath);
        if(descriptor.thumbnail != nullptr)
        {
            const auto texture = descriptor.thumbnail;
            const auto sprite = UnityEngine::Sprite::Create(texture, UnityEngine::Rect(0.0f, 0.0f, static_cast<float>(texture->get_width()), static_cast<float>(texture->get_height())), UnityEngine::Vector2(0.5f, 0.5f), 1024.0f, 1u, UnityEngine::SpriteMeshType::FullRect, UnityEngine::Vector4(0.0f, 0.0f, 0.0f, 0.0f), false);
            l_TextListCell->Image->Element()->SetSprite(sprite);
        }
        else
        {
            l_TextListCell->Image->Element()->SetSprite(cdsprite);
        }
    }
}
