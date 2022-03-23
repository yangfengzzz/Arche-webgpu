//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_properties.h"
#include "editor_actions.h"
#include "filesystem.h"

#include "ui/widgets/visual/separator.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/layout/group_collapsable.h"
#include "ui/widgets/layout/new_line.h"
#include "ui/widgets/selection/combo_box.h"
#include "ui/gui_drawer.h"

#include "size_converter.h"

namespace vox {
namespace editor {
namespace ui {
AssetProperties::AssetProperties(const std::string &p_title, bool p_opened,
                                 const PanelWindowSettings &p_windowSettings,
                                 AssetView &view) :
PanelWindow(p_title, p_opened, p_windowSettings),
_assetView(view) {
    _targetChanged += [this]() {
        setTarget(_assetSelector->content);
    };
    
    createHeaderButtons();
    
    _headerSeparator = &createWidget<::vox::ui::Separator>();
    _headerSeparator->enabled = false;
    
    createAssetSelector();
    
    _settings = &createWidget<GroupCollapsable>("Settings");
    _settingsColumns = &_settings->createWidget<Columns<2>>();
    _settingsColumns->widths[0] = 150;
    
    _info = &createWidget<GroupCollapsable>("Info");
    _infoColumns = &_info->createWidget<Columns<2>>();
    _infoColumns->widths[0] = 150;
    
    _settings->enabled = _info->enabled = false;
}

void AssetProperties::setTarget(const std::string &p_path) {
    _resource = p_path == "" ? p_path : EditorActions::getSingleton().getResourcePath(p_path);
    
    if (_assetSelector) {
        _assetSelector->content = _resource;
    }
    
    refresh();
}

void AssetProperties::refresh() {
    _metadata = std::make_unique<::vox::fs::IniFile>(EditorActions::getSingleton().getRealPath(_resource) + ".meta");
    
    createSettings();
    createInfo();
    
    _applyButton->enabled = _settings->enabled;
    _resetButton->enabled = _settings->enabled;
    _revertButton->enabled = _settings->enabled;
    
    switch (fs::extraFileType(_resource)) {
        case fs::FileType::MODEL:
        case fs::FileType::TEXTURE:
        case fs::FileType::MATERIAL:
            _previewButton->enabled = true;
            break;
        default:
            _previewButton->enabled = false;
            break;
    }
    
    // Enables the header separator (And the line break) if at least one button is enabled
    _headerSeparator->enabled = _applyButton->enabled || _resetButton->enabled || _revertButton->enabled || _previewButton->enabled;
    _headerLineBreak->enabled = _headerSeparator->enabled;
}

void AssetProperties::preview() {
    const auto fileType = fs::extraFileType(_resource);
    
    if (fileType == fs::FileType::MODEL) {
        // if (auto resource = OVSERVICE(OvCore::ResourceManagement::ModelManager).GetResource(resource)) {
        //     _assetView.setResource(resource);
        // }
    } else if (fileType == fs::FileType::TEXTURE) {
        // if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(resource)) {
        //     _assetView.setResource(resource);
        // }
    }
    
    _assetView.open();
}

void AssetProperties::createHeaderButtons() {
    _applyButton = &createWidget<ButtonSimple>("Apply");
    _applyButton->idleBackgroundColor = {0.0f, 0.5f, 0.0f};
    _applyButton->enabled = false;
    _applyButton->lineBreak = false;
    _applyButton->clickedEvent += std::bind(&AssetProperties::apply, this);
    
    _revertButton = &createWidget<ButtonSimple>("Revert");
    _revertButton->idleBackgroundColor = {0.7f, 0.5f, 0.0f};
    _revertButton->enabled = false;
    _revertButton->lineBreak = false;
    _revertButton->clickedEvent += std::bind(&AssetProperties::setTarget, this, _resource);
    
    _previewButton = &createWidget<ButtonSimple>("Preview");
    _previewButton->idleBackgroundColor = {0.7f, 0.5f, 0.0f};
    _previewButton->enabled = false;
    _previewButton->lineBreak = false;
    _previewButton->clickedEvent += std::bind(&AssetProperties::preview, this);
    
    _resetButton = &createWidget<ButtonSimple>("Reset to default");
    _resetButton->idleBackgroundColor = {0.5f, 0.0f, 0.0f};
    _resetButton->enabled = false;
    _resetButton->lineBreak = false;
    _resetButton->clickedEvent += [this] {
        _metadata->removeAll();
        createSettings();
    };
    
    _headerLineBreak = &createWidget<NewLine>();
    _headerLineBreak->enabled = false;
}

void AssetProperties::createAssetSelector() {
    auto &columns = createWidget<Columns<2>>();
    columns.widths[0] = 150;
    // assetSelector = &OvCore::Helpers::GUIDrawer::DrawAsset(columns, "Target", resource, &targetChanged);
}

void AssetProperties::createSettings() {
    _settingsColumns->removeAllWidgets();
    
    const auto fileType = fs::extraFileType(_resource);
    
    _settings->enabled = true;
    
    if (fileType == fs::FileType::MODEL) {
        createModelSettings();
    } else if (fileType == fs::FileType::TEXTURE) {
        createTextureSettings();
    } else {
        _settings->enabled = false;
    }
}

void AssetProperties::createInfo() {
    const auto realPath = EditorActions::getSingleton().getRealPath(_resource);
    
    _infoColumns->removeAllWidgets();
    
    if (std::filesystem::exists(realPath)) {
        _info->enabled = true;
        
        GUIDrawer::createTitle(*_infoColumns, "Path");
        _infoColumns->createWidget<Text>(realPath);
        
        GUIDrawer::createTitle(*_infoColumns, "Size");
        const auto[size, unit] = SizeConverter::convertToOptimalUnit(static_cast<float>(std::filesystem::file_size(realPath)),
                                                                     SizeConverter::SizeUnit::BYTE);
        _infoColumns->createWidget<Text>(std::to_string(size) + " " + SizeConverter::unitToString(unit));
        
        GUIDrawer::createTitle(*_infoColumns, "Metadata");
        _infoColumns->createWidget<Text>(std::filesystem::exists(realPath + ".meta") ? "Yes" : "No");
    } else {
        _info->enabled = false;
    }
}

#define MODEL_FLAG_ENTRY(setting) GUIDrawer::drawBoolean(*_settingsColumns, setting, [&]() { \
return _metadata->get<bool>(setting); }, [&](bool value) { _metadata->set<bool>(setting, value); })

void AssetProperties::createModelSettings() {
    _metadata->add("CALC_TANGENT_SPACE", true);
    _metadata->add("JOIN_IDENTICAL_VERTICES", true);
    _metadata->add("MAKE_LEFT_HANDED", false);
    _metadata->add("TRIANGULATE", true);
    _metadata->add("REMOVE_COMPONENT", false);
    _metadata->add("GEN_NORMALS", false);
    _metadata->add("GEN_SMOOTH_NORMALS", true);
    _metadata->add("SPLIT_LARGE_MESHES", false);
    _metadata->add("PRE_TRANSFORVERTICES", true);
    _metadata->add("LIMIT_BONE_WEIGHTS", false);
    _metadata->add("VALIDATE_DATA_STRUCTURE", false);
    _metadata->add("IMPROVE_CACHE_LOCALITY", true);
    _metadata->add("REMOVE_REDUNDANT_MATERIALS", false);
    _metadata->add("FIX_INFACING_NORMALS", false);
    _metadata->add("SORT_BY_PTYPE", false);
    _metadata->add("FIND_DEGENERATES", false);
    _metadata->add("FIND_INVALID_DATA", true);
    _metadata->add("GEN_UV_COORDS", true);
    _metadata->add("TRANSFORUV_COORDS", false);
    _metadata->add("FIND_INSTANCES", true);
    _metadata->add("OPTIMIZE_MESHES", true);
    _metadata->add("OPTIMIZE_GRAPH", true);
    _metadata->add("FLIP_UVS", false);
    _metadata->add("FLIP_WINDING_ORDER", false);
    _metadata->add("SPLIT_BY_BONE_COUNT", false);
    _metadata->add("DEBONE", true);
    _metadata->add("GLOBAL_SCALE", true);
    _metadata->add("EMBED_TEXTURES", false);
    _metadata->add("FORCE_GEN_NORMALS", false);
    _metadata->add("DROP_NORMALS", false);
    _metadata->add("GEN_BOUNDING_BOXES", false);
    
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
    // _metadata->add("MIN_FILTER", static_cast<int>(OvRendering::Settings::ETextureFilteringMode::LINEAR_MIPMAP_LINEAR));
    // _metadata->add("MAG_FILTER", static_cast<int>(OvRendering::Settings::ETextureFilteringMode::LINEAR));
    _metadata->add("ENABLE_MIPMAPPING", true);
    
    std::map<int, std::string> filteringModes
    {
        {0x2600, "NEAREST"},
        {0x2601, "LINEAR"},
        {0x2700, "NEAREST_MIPMAP_NEAREST"},
        {0x2703, "LINEAR_MIPMAP_LINEAR"},
        {0x2701, "LINEAR_MIPMAP_NEAREST"},
        {0x2702, "NEAREST_MIPMAP_LINEAR"}
    };
    
    GUIDrawer::createTitle(*_settingsColumns, "MIN_FILTER");
    auto &minFilter = _settingsColumns->createWidget<ComboBox>(_metadata->get<int>("MIN_FILTER"));
    minFilter.choices = filteringModes;
    minFilter.valueChangedEvent += [this](int p_choice) {
        _metadata->set("MIN_FILTER", p_choice);
    };
    
    GUIDrawer::createTitle(*_settingsColumns, "MAG_FILTER");
    auto &magFilter = _settingsColumns->createWidget<ComboBox>(_metadata->get<int>("MAG_FILTER"));
    magFilter.choices = filteringModes;
    magFilter.valueChangedEvent += [this](int p_choice) {
        _metadata->set("MAG_FILTER", p_choice);
    };
    
    GUIDrawer::drawBoolean(*_settingsColumns, "ENABLE_MIPMAPPING", [&]() {
        return _metadata->get<bool>("ENABLE_MIPMAPPING");
    }, [&](bool value) {
        _metadata->set<bool>("ENABLE_MIPMAPPING", value);
    });
}

void AssetProperties::apply() {
    _metadata->rewrite();
    
    const auto resourcePath = EditorActions::getSingleton().getResourcePath(_resource);
    const auto fileType = fs::extraFileType(_resource);
    
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


}
}
}
