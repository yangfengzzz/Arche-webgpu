//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef panel_window_settings_h
#define panel_window_settings_h

namespace vox {
namespace ui {
/**
 * Data structure to send to the panel window constructor to define its settings
 */
struct PanelWindowSettings {
    bool closable = false;
    bool resizable = true;
    bool movable = true;
    bool dockable = false;
    bool scrollable = true;
    bool hideBackground = false;
    bool forceHorizontalScrollbar = false;
    bool forceVerticalScrollbar = false;
    bool allowHorizontalScrollbar = false;
    bool bringToFrontOnFocus = true;
    bool collapsable = false;
    bool allowInputs = true;
    bool titleBar = true;
    bool autoSize = false;
};


}
}
#endif /* panel_window_settings_h */
