#pragma once

#include "vrm/VRMDescriptor.hpp"
#include "HMUI/TableCell.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableView_IDataSource.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Sprite.hpp"
#include "custom-types/shared/macros.hpp"
#include <functional>
#include <set>

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, method, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, method, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif

___DECLARE_TYPE_WRAPPER_INHERITANCE(VRMQavatars::UI::Components, AvatarListTableData, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, UnityEngine::MonoBehaviour, "VRMQavatars::UI::Components", {classof(HMUI::TableView::IDataSource*)}, 0, nullptr,
        
    DECLARE_INSTANCE_FIELD(HMUI::TableView*, tableView);
    DECLARE_INSTANCE_FIELD(Il2CppString*, reuseIdentifier);
    DECLARE_INSTANCE_FIELD(float, cellSize);

    DECLARE_CTOR(ctor);

    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_INSTANCE_METHOD(void, DidSelectCellWithIdx, HMUI::TableView* tableView, int idx);

    DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);
    DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);
    public:
    std::function<void(HMUI::TableCell*)> onSelect = nullptr;
    std::set<VRMQavatars::VRMDescriptor, VRMQavatars::AvatarComparator> avatarDescriptors = {};
)