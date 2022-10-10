//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/asset_properties.h"

#include "vox.editor/size_converter.h"
#include "vox.editor/editor_actions.h"
#include "vox.render/ui/gui_drawer.h"
#include "vox.render/ui/widgets/layout/group_collapsable.h"
#include "vox.render/ui/widgets/layout/new_line.h"
#include "vox.render/ui/widgets/selection/combo_box.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
AssetProperties::AssetProperties(const std::string &p_title,
                                 bool p_opened,
                                 const PanelWindowSettings &p_windowSettings,
                                 AssetView &view)
    : PanelWindow(p_title, p_opened, p_windowSettings), _assetView(view) {
    _targetChanged += [this]() { setTarget(_assetSelector->content_); };

    createHeaderButtons();

    _headerSeparator = &CreateWidget<::vox::ui::Separator>();
    _headerSeparator->enabled_ = false;

    createAssetSelector();

    _settings = &CreateWidget<GroupCollapsable>("Settings");
    _settingsColumns = &_settings->CreateWidget<Columns<2>>();
    _settingsColumns->widths_[0] = 150;

    _info = &CreateWidget<GroupCollapsable>("Info");
    _infoColumns = &_info->CreateWidget<Columns<2>>();
    _infoColumns->widths_[0] = 150;

    _settings->enabled_ = _info->enabled_ = false;
}

void AssetProperties::setTarget(const std::string &p_path) {
    _resource = p_path.empty() ? p_path : EditorActions::getSingleton().getResourcePath(p_path);

    if (_assetSelector) {
        _assetSelector->content_ = _resource;
    }

    refresh();
}

void AssetProperties::refresh() {
    _metadata = std::make_unique<::vox::fs::IniFile>(EditorActions::getSingleton().getRealPath(_resource) + ".meta");

    createSettings();
    createInfo();

    _applyButton->enabled_ = _settings->enabled_;
    _resetButton->enabled_ = _settings->enabled_;
    _revertButton->enabled_ = _settings->enabled_;

    switch (fs::ExtraFileType(_resource)) {
        case fs::FileType::MODEL:
        case fs::FileType::TEXTURE:
        case fs::FileType::MATERIAL:
            _previewButton->enabled_ = true;
            break;
        default:
            _previewButton->enabled_ = false;
            break;
    }

    // Enables the header separator (And the line break) if at least one button is enabled_
    _headerSeparator->enabled_ =
            _applyButton->enabled_ || _resetButton->enabled_ || _revertButton->enabled_ || _previewButton->enabled_;
    _headerLineBreak->enabled_ = _headerSeparator->enabled_;
}

void AssetProperties::preview() {
    const auto fileType = fs::ExtraFileType(_resource);

    if (fileType == fs::FileType::MODEL) {
        // if (auto resource = OVSERVICE(OvCore::ResourceManagement::ModelManager).GetResource(resource)) {
        //     _assetView.setResource(resource);
        // }
    } else if (fileType == fs::FileType::TEXTURE) {
        // if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(resource)) {
        //     _assetView.setResource(resource);
        // }
    }

    _assetView.Open();
}

void AssetProperties::createHeaderButtons() {
    _applyButton = &CreateWidget<ButtonSimple>("Apply");
    _applyButton->idle_background_color_ = {0.0f, 0.5f, 0.0f};
    _applyButton->enabled_ = false;
    _applyButton->line_break_ = false;
    _applyButton->clicked_event_ += std::bind(&AssetProperties::apply, this);

    _revertButton = &CreateWidget<ButtonSimple>("Revert");
    _revertButton->idle_background_color_ = {0.7f, 0.5f, 0.0f};
    _revertButton->enabled_ = false;
    _revertButton->line_break_ = false;
    _revertButton->clicked_event_ += std::bind(&AssetProperties::setTarget, this, _resource);

    _previewButton = &CreateWidget<ButtonSimple>("Preview");
    _previewButton->idle_background_color_ = {0.7f, 0.5f, 0.0f};
    _previewButton->enabled_ = false;
    _previewButton->line_break_ = false;
    _previewButton->clicked_event_ += std::bind(&AssetProperties::preview, this);

    _resetButton = &CreateWidget<ButtonSimple>("Reset to default");
    _resetButton->idle_background_color_ = {0.5f, 0.0f, 0.0f};
    _resetButton->enabled_ = false;
    _resetButton->line_break_ = false;
    _resetButton->clicked_event_ += [this] {
        _metadata->RemoveAll();
        createSettings();
    };

    _headerLineBreak = &CreateWidget<NewLine>();
    _headerLineBreak->enabled_ = false;
}

void AssetProperties::createAssetSelector() {
    auto &columns = CreateWidget<Columns<2>>();
    columns.widths_[0] = 150;
    // assetSelector = &OvCore::Helpers::GuiDrawer::DrawAsset(columns, "Target", resource, &targetChanged);
}

void AssetProperties::createSettings() {
    _settingsColumns->RemoveAllWidgets();

    const auto fileType = fs::ExtraFileType(_resource);

    _settings->enabled_ = true;

    if (fileType == fs::FileType::MODEL) {
        createModelSettings();
    } else if (fileType == fs::FileType::TEXTURE) {
        createTextureSettings();
    } else {
        _settings->enabled_ = false;
    }
}

void AssetProperties::createInfo() {
    const auto realPath = EditorActions::getSingleton().getRealPath(_resource);

    _infoColumns->RemoveAllWidgets();

    if (std::filesystem::exists(realPath)) {
        _info->enabled_ = true;

        GuiDrawer::CreateTitle(*_infoColumns, "Path");
        _infoColumns->CreateWidget<Text>(realPath);

        GuiDrawer::CreateTitle(*_infoColumns, "Size");
        const auto [size, unit] = SizeConverter::convertToOptimalUnit(
                static_cast<float>(std::filesystem::file_size(realPath)), SizeConverter::SizeUnit::BYTE);
        _infoColumns->CreateWidget<Text>(std::to_string(size) + " " + SizeConverter::unitToString(unit));

        GuiDrawer::CreateTitle(*_infoColumns, "Metadata");
        _infoColumns->CreateWidget<Text>(std::filesystem::exists(realPath + ".meta") ? "Yes" : "No");
    } else {
        _info->enabled_ = false;
    }
}

#define MODEL_FLAG_ENTRY(setting)                                                        \
    GuiDrawer::DrawBoolean(                                                              \
            *_settingsColumns, setting, [&]() { return _metadata->Get<bool>(setting); }, \
            [&](bool value) { _metadata->Set<bool>(setting, value); })

void AssetProperties::createModelSettings() {
    _metadata->Add("CALC_TANGENT_SPACE", true);
    _metadata->Add("JOIN_IDENTICAL_VERTICES", true);
    _metadata->Add("MAKE_LEFT_HANDED", false);
    _metadata->Add("TRIANGULATE", true);
    _metadata->Add("REMOVE_COMPONENT", false);
    _metadata->Add("GEN_NORMALS", false);
    _metadata->Add("GEN_SMOOTH_NORMALS", true);
    _metadata->Add("SPLIT_LARGE_MESHES", false);
    _metadata->Add("PRE_TRANSFORVERTICES", true);
    _metadata->Add("LIMIT_BONE_WEIGHTS", false);
    _metadata->Add("VALIDATE_DATA_STRUCTURE", false);
    _metadata->Add("IMPROVE_CACHE_LOCALITY", true);
    _metadata->Add("REMOVE_REDUNDANT_MATERIALS", false);
    _metadata->Add("FIX_INFACING_NORMALS", false);
    _metadata->Add("SORT_BY_PTYPE", false);
    _metadata->Add("FIND_DEGENERATES", false);
    _metadata->Add("FIND_INVALID_DATA", true);
    _metadata->Add("GEN_UV_COORDS", true);
    _metadata->Add("TRANSFORUV_COORDS", false);
    _metadata->Add("FIND_INSTANCES", true);
    _metadata->Add("OPTIMIZE_MESHES", true);
    _metadata->Add("OPTIMIZE_GRAPH", true);
    _metadata->Add("FLIP_UVS", false);
    _metadata->Add("FLIP_WINDING_ORDER", false);
    _metadata->Add("SPLIT_BY_BONE_COUNT", false);
    _metadata->Add("DEBONE", true);
    _metadata->Add("GLOBAL_SCALE", true);
    _metadata->Add("EMBED_TEXTURES", false);
    _metadata->Add("FORCE_GEN_NORMALS", false);
    _metadata->Add("DROP_NORMALS", false);
    _metadata->Add("GEN_BOUNDING_BOXES", false);

    MODEL_FLAG_ENTRY("CALC_TANGENT_SPACE");
    MODEL_FLAG_ENTRY("JOIN_IDENTICAL_VERTICES");
    MODEL_FLAG_ENTRY("MAKE_LEFT_HANDED");
    MODEL_FLAG_ENTRY("TRIANGULATE");
    MODEL_FLAG_ENTRY("REMOVE_COMPONENT");
    MODEL_FLAG_ENTRY("GEN_NORMALS");
    MODEL_FLAG_ENTRY("GEN_SMOOTH_NORMALS");
    MODEL_FLAG_ENTRY("SPLIT_LARGE_MESHES");
    MODEL_FLAG_ENTRY("PRE_TRANSFORVERTICES");
    MODEL_FLAG_ENTRY("LIMIT_BONE_WEIGHTS");
    MODEL_FLAG_ENTRY("VALIDATE_DATA_STRUCTURE");
    MODEL_FLAG_ENTRY("IMPROVE_CACHE_LOCALITY");
    MODEL_FLAG_ENTRY("REMOVE_REDUNDANT_MATERIALS");
    MODEL_FLAG_ENTRY("FIX_INFACING_NORMALS");
    MODEL_FLAG_ENTRY("SORT_BY_PTYPE");
    MODEL_FLAG_ENTRY("FIND_DEGENERATES");
    MODEL_FLAG_ENTRY("FIND_INVALID_DATA");
    MODEL_FLAG_ENTRY("GEN_UV_COORDS");
    MODEL_FLAG_ENTRY("TRANSFORUV_COORDS");
    MODEL_FLAG_ENTRY("FIND_INSTANCES");
    MODEL_FLAG_ENTRY("OPTIMIZE_MESHES");
    MODEL_FLAG_ENTRY("OPTIMIZE_GRAPH");
    MODEL_FLAG_ENTRY("FLIP_UVS");
    MODEL_FLAG_ENTRY("FLIP_WINDING_ORDER");
    MODEL_FLAG_ENTRY("SPLIT_BY_BONE_COUNT");
    MODEL_FLAG_ENTRY("DEBONE");
    MODEL_FLAG_ENTRY("GLOBAL_SCALE");
    MODEL_FLAG_ENTRY("EMBED_TEXTURES");
    MODEL_FLAG_ENTRY("FORCE_GEN_NORMALS");
    MODEL_FLAG_ENTRY("DROP_NORMALS");
    MODEL_FLAG_ENTRY("GEN_BOUNDING_BOXES");
};

void AssetProperties::createTextureSettings() {
    // _metadata->Add("MIN_FILTER",
    // static_cast<int>(OvRendering::Settings::ETextureFilteringMode::LINEAR_MIPMAP_LINEAR));
    // _metadata->Add("MAG_FILTER", static_cast<int>(OvRendering::Settings::ETextureFilteringMode::LINEAR));
    _metadata->Add("ENABLE_MIPMAPPING", true);

    std::map<int, std::string> filteringModes{{0x2600, "NEAREST"},
                                              {0x2601, "LINEAR"},
                                              {0x2700, "NEAREST_MIPMAP_NEAREST"},
                                              {0x2703, "LINEAR_MIPMAP_LINEAR"},
                                              {0x2701, "LINEAR_MIPMAP_NEAREST"},
                                              {0x2702, "NEAREST_MIPMAP_LINEAR"}};

    GuiDrawer::CreateTitle(*_settingsColumns, "MIN_FILTER");
    auto &minFilter = _settingsColumns->CreateWidget<ComboBox>(_metadata->Get<int>("MIN_FILTER"));
    minFilter.choices_ = filteringModes;
    minFilter.value_changed_event_ += [this](int p_choice) { _metadata->Set("MIN_FILTER", p_choice); };

    GuiDrawer::CreateTitle(*_settingsColumns, "MAG_FILTER");
    auto &magFilter = _settingsColumns->CreateWidget<ComboBox>(_metadata->Get<int>("MAG_FILTER"));
    magFilter.choices_ = filteringModes;
    magFilter.value_changed_event_ += [this](int p_choice) { _metadata->Set("MAG_FILTER", p_choice); };

    GuiDrawer::DrawBoolean(
            *_settingsColumns, "ENABLE_MIPMAPPING", [&]() { return _metadata->Get<bool>("ENABLE_MIPMAPPING"); },
            [&](bool value) { _metadata->Set<bool>("ENABLE_MIPMAPPING", value); });
}

void AssetProperties::apply() {
    _metadata->Rewrite();

    const auto resourcePath = EditorActions::getSingleton().getResourcePath(_resource);
    const auto fileType = fs::ExtraFileType(_resource);

    //    if (fileType == fs::FileType::MODEL) {
    //        auto &modelManager = OVSERVICE(OvCore::ResourceManagement::ModelManager);
    //        if (modelManager.IsResourceRegistered(resourcePath)) {
    //            modelManager.AResourceManager::ReloadResource(resourcePath);
    //        }
    //    } else if (fileType == fs::FileType::TEXTURE) {
    //        auto &textureManager = OVSERVICE(OvCore::ResourceManagement::TextureManager);
    //        if (textureManager.IsResourceRegistered(resourcePath)) {
    //            textureManager.AResourceManager::ReloadResource(resourcePath);
    //        }
    //    }

    refresh();
}

}  // namespace vox::editor::ui
