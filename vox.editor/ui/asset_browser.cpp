//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/asset_browser.h"

namespace vox::editor::ui {
AssetBrowser::AssetBrowser(const std::string &title,
                           bool opened,
                           const ::vox::ui::PanelWindowSettings &window_settings,
                           const std::string &engine_asset_folder,
                           const std::string &project_asset_folder,
                           const std::string &project_script_folder) {}

void AssetBrowser::fill() {}

void AssetBrowser::clear() {}

void AssetBrowser::refresh() {}

void AssetBrowser::parseFolder(::vox::ui::TreeNode &root,
                               const std::filesystem::directory_entry &directory,
                               bool is_engine_item,
                               bool script_folder) {}

void AssetBrowser::considerItem(::vox::ui::TreeNode *root,
                                const std::filesystem::directory_entry &entry,
                                bool is_engine_item,
                                bool auto_open,
                                bool script_folder) {}
}  // namespace vox::editor::ui
