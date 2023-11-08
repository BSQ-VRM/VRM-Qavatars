#pragma once

#include "vrm/VRMDescriptor.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/TableCell.hpp"
#include "AvatarListTableData.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/EventSystems/BaseEventData.hpp"
#include "UnityEngine/EventSystems/PointerEventData.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include <functional>

#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "bsml/shared/BSML/Components/ClickableImage.hpp"

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, method, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, method, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif

DECLARE_CLASS_CODEGEN(VRMQavatars::UI::Components, AvatarListTableCell, HMUI::TableCell,

    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, name);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, sub);
    DECLARE_INSTANCE_FIELD(BSML::Backgroundable*, backgroundImage);

    DECLARE_INSTANCE_FIELD(AvatarListTableData*, tableData);

    DECLARE_INSTANCE_METHOD(void, PostParse);
    DECLARE_INSTANCE_METHOD(void, Select);
    DECLARE_OVERRIDE_METHOD_MATCH(void, HighlightDidChange, &HMUI::SelectableCell::HighlightDidChange, HMUI::SelectableCell::TransitionType transitionType);

    public:

    static VRMQavatars::UI::Components::AvatarListTableCell* CreateNewCell();

    void SetDescriptor(VRMQavatars::VRMDescriptor descriptor);
    void set_name(std::string_view name);
    void set_sub(std::string_view sub);
    VRMQavatars::VRMDescriptor descriptor;

    std::function<void(AvatarListTableCell*)> onSelect;
)