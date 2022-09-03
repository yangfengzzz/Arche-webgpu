//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox::ui {
/**
 * Data structure to send to the panel window constructor to define its settings
 */
struct PanelWindowSettings {
    bool closable = false;
    bool resizable = true;
    bool movable = true;
    bool dockable = false;
    bool scrollable = true;
    bool hide_background = false;
    bool force_horizontal_scrollbar = false;
    bool force_vertical_scrollbar = false;
    bool allow_horizontal_scrollbar = false;
    bool bring_to_front_on_focus = true;
    bool collapsable = false;
    bool allow_inputs = true;
    bool title_bar = true;
    bool auto_size = false;
};

}  // namespace vox::ui
