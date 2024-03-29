//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <filesystem>
#include <queue>
#include <unordered_map>

#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/layout/tree_node.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"

namespace vox::editor::ui {
/**
 * A panel that handle asset management
 */
class AssetBrowser : public ::vox::ui::PanelWindow {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @parma p_opened p_opened
     * @param p_windowSettings p_windowSettings
     * @param p_engineAssetFolder p_engineAssetFolder
     * @param p_projectAssetFolder p_projectAssetFolder
     * @param p_projectScriptFolder p_projectScriptFolder
     */
    AssetBrowser(const std::string &p_title,
                 bool p_opened,
                 const ::vox::ui::PanelWindowSettings &p_windowSettings,
                 const std::string &p_engineAssetFolder = "",
                 const std::string &p_projectAssetFolder = "",
                 const std::string &p_projectScriptFolder = "");

    /**
     * Fill the asset browser panels with widgets corresponding to elements in the asset folder
     */
    void fill();

    /**
     * Clear the asset browser widgets
     */
    void clear();

    /**
     * Refresh the asset browser widgets (Clear + Fill)
     */
    void refresh();

private:
    void parseFolder(::vox::ui::TreeNode &p_root,
                     const std::filesystem::directory_entry &p_directory,
                     bool p_isEngineItem,
                     bool p_scriptFolder = false);

    void considerItem(::vox::ui::TreeNode *p_root,
                      const std::filesystem::directory_entry &p_entry,
                      bool p_isEngineItem,
                      bool p_autoOpen = false,
                      bool p_scriptFolder = false);

public:
    static const std::string __FILENAMES_CHARS;

private:
    std::string _engineAssetFolder;
    std::string _projectAssetFolder;
    std::string _projectScriptFolder;
    ::vox::ui::Group *_assetList;
    std::unordered_map<::vox::ui::TreeNode *, std::string> _pathUpdate;
};

}  // namespace vox::editor::ui