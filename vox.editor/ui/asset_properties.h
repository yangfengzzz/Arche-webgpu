//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.editor/ini_file.h"
#include "vox.editor/view/asset_view.h"
#include "vox.render/event.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/layout/columns.h"
#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/texts/text.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class AssetProperties : public PanelWindow {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    AssetProperties(const std::string &p_title,
                    bool p_opened,
                    const PanelWindowSettings &p_windowSettings,
                    AssetView &view);

    /**
     * Defines the target of the asset settings editor
     * @param p_path p_path
     */
    void setTarget(const std::string &p_path);

    /**
     * Refresh the panel to show the current target settings
     */
    void refresh();

    /**
     * Launch the preview of the target asset
     */
    void preview();

private:
    void createHeaderButtons();

    void createAssetSelector();

    void createSettings();

    void createInfo();

    void createModelSettings();

    void createTextureSettings();

    void apply();

private:
    AssetView &_assetView;
    std::string _resource;

    Event<> _targetChanged;
    Group *_settings{nullptr};
    Group *_info{nullptr};
    ButtonSimple *_applyButton{nullptr};
    ButtonSimple *_revertButton{nullptr};
    ButtonSimple *_previewButton{nullptr};
    ButtonSimple *_resetButton{nullptr};
    Widget *_headerSeparator{nullptr};
    Widget *_headerLineBreak{nullptr};
    Columns<2> *_settingsColumns{nullptr};
    Columns<2> *_infoColumns{nullptr};
    Text *_assetSelector{nullptr};
    std::unique_ptr<fs::IniFile> _metadata{nullptr};
};

}  // namespace editor::ui
}  // namespace vox