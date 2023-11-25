#include <chatplex-sdk-bs/shared/CP_SDK/UI/UISystem.hpp>

#include "UI/components/AvatarListItem.hpp"
#include "UI/components/AvatarListCell.hpp"

#include <chatplex-sdk-bs/shared/CP_SDK/XUI/XUI.hpp>
#include <questui/shared/BeatSaberUI.hpp>

#include "assets.hpp"
#include "System/Action_2.hpp"

#include "bsml/shared/Helpers/utilities.hpp"

#include "custom-types/shared/delegate.hpp"

namespace VRMQavatars::UI::Components {

    CP_SDK_IL2CPP_INHERIT_INIT(AvatarListCell);

    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(AvatarListCell)
    {
        m_Build         = {this, &AvatarListCell::Build_Impl};
        m_GetCellHeight = {this, &AvatarListCell::GetCellHeight_Impl};

        m_AddSelfComponent = {this, &AvatarListCell::AddSelfComponent_Impl};
    }

    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(AvatarListCell)
    {

    }

    void AvatarListCell::Build_Impl()
    {
        static auto sprite = QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::cd_png);
        CP_SDK::XUI::XUIHLayout::Make({
            CP_SDK::XUI::XUIImage::Make()
                ->Bind(&Image)
                ->SetSprite(sprite)
                ->OnReady([](CP_SDK::UI::Components::CImage* img)
                {
                    img->RTransform()->set_anchoredPosition({-33, 0});
                    img->RTransform()->set_sizeDelta(UnityEngine::Vector2::get_zero());
                    img->LElement()->set_ignoreLayout(true);
                })
                ->SetWidth(6.5f)
                ->SetHeight(6.5f)
                ->AsShared(),
            CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"Name")
                    ->Bind(&NameText)
                    ->OnReady([](CP_SDK::UI::Components::CText* text)
                    {
                        //silly chars are broken on chatplex base font
                        text->TMProUGUI()->set_font(QuestUI::BeatSaberUI::GetMainTextFont());
                        text->TMProUGUI()->set_fontMaterial(QuestUI::BeatSaberUI::GetMainUIFontMaterial());
                        text->RTransform()->set_sizeDelta(UnityEngine::Vector2::get_zero());
                    })
                    ->SetAlign(TMPro::TextAlignmentOptions::CaplineLeft)
                    ->SetFontSize(4.0f)
                    ->AsShared(),
                CP_SDK::XUI::XUIText::Make(u"Author")
                    ->Bind(&AuthorText)
                    ->OnReady([](CP_SDK::UI::Components::CText* text)
                    {
                        //silly chars are broken on chatplex base font
                        text->TMProUGUI()->set_font(QuestUI::BeatSaberUI::GetMainTextFont());
                        text->TMProUGUI()->set_fontMaterial(QuestUI::BeatSaberUI::GetMainUIFontMaterial());
                        text->RTransform()->set_sizeDelta(UnityEngine::Vector2::get_zero());
                    })
                    ->SetAlign(TMPro::TextAlignmentOptions::CaplineLeft)
                    ->SetFontSize(4.0f)
                    ->AsShared()
            })
            ->OnReady([](CP_SDK::UI::Components::CVLayout* layout)
            {
                layout->CSizeFitter()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
                layout->CSizeFitter()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
                layout->RTransform()->set_anchoredPosition({-5, 0});
                layout->LElement()->set_ignoreLayout(true);
                layout->HOrVLayoutGroup()->set_childAlignment(UnityEngine::TextAnchor::MiddleLeft);
                layout->HOrVLayoutGroup()->set_spacing(-2);
                layout->LElement()->set_preferredWidth(50.0f);
            })
            ->AsShared(),
            CP_SDK::XUI::XUIVLayout::Make(
            {
                CP_SDK::XUI::XUIText::Make(u"Name")
                    ->Bind(&FilenameText)
                    ->OnReady([](CP_SDK::UI::Components::CText* text)
                    {
                        //silly chars are broken on chatplex base font
                        text->TMProUGUI()->set_font(QuestUI::BeatSaberUI::GetMainTextFont());
                        text->TMProUGUI()->set_fontMaterial(QuestUI::BeatSaberUI::GetMainUIFontMaterial());
                        text->RTransform()->set_sizeDelta(UnityEngine::Vector2::get_zero());
                    })
                    ->SetAlign(TMPro::TextAlignmentOptions::CaplineRight)
                    ->SetFontSize(3.0f)
                    ->SetColor({0.8f, 0.8f, 0.8f, 0.8f})
                    ->AsShared()
            })
            ->OnReady([](CP_SDK::UI::Components::CVLayout* layout)
            {
                layout->CSizeFitter()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
                layout->CSizeFitter()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
                layout->RTransform()->set_anchoredPosition({22, 0});
                layout->LElement()->set_ignoreLayout(true);
                layout->HOrVLayoutGroup()->set_childAlignment(UnityEngine::TextAnchor::MiddleRight);
                layout->LElement()->set_preferredWidth(30.0f);
            })
            ->AsShared()
        })
        ->OnReady([](CP_SDK::UI::Components::CHLayout* layout)
        {
            layout->CSizeFitter()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            layout->CSizeFitter()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            layout->HOrVLayoutGroup()->set_childAlignment(UnityEngine::TextAnchor::MiddleLeft);
        })
        ->AsShared()
        ->BuildUI(RTransform().Ptr());
    }

    float AvatarListCell::GetCellHeight_Impl()
    {
        return 8;
    }

    CP_SDK::UI::Data::IListCell* AvatarListCell::AddSelfComponent_Impl(UnityEngine::GameObject* p_Target)
    {
        auto l_Existing = p_Target->GetComponent<AvatarListCell*>();
        if (!l_Existing)
            l_Existing = p_Target->AddComponent<AvatarListCell*>();

        return l_Existing;
    }

}