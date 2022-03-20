//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef panel_window_hpp
#define panel_window_hpp

#include "ui/widgets/panel_transformable.h"
#include "ui/widgets/panel_window_settings.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * A PanelWindow is a panel that is localized and behave like a window (Movable, resizable...)
 */
class PanelWindow : public PanelTransformable {
public:
    /**
     * Creates the PanelWindow
     * @param p_name p_name
     * @param p_opened p_opened
     * @param p_panelSettings p_panelSettings
     */
    PanelWindow(const std::string &p_name = "",
                bool p_opened = true,
                const PanelWindowSettings &p_panelSettings = PanelWindowSettings{});
    
    /**
     * Open (show) the panel
     */
    void open();
    
    /**
     * Close (hide) the panel
     */
    void close();
    
    /**
     * Focus the panel
     */
    void focus();
    
    /**
     * Defines the opened state of the window
     * @param p_value p_value
     */
    void setOpened(bool p_value);
    
    /**
     * Returns true if the panel is opened
     */
    bool isOpened() const;
    
    /**
     * Returns true if the panel is hovered
     */
    bool isHovered() const;
    
    /**
     * Returns true if the panel is focused
     */
    bool isFocused() const;
    
    /**
     * Returns true if the panel is appearing
     */
    bool isAppearing() const;
    
    /**
     * Scrolls to the bottom of the window
     */
    void scrollToBottom();
    
    /**
     * Scrolls to the top of the window
     */
    void scrollToTop();
    
    /**
     * Returns true if the window is scrolled to the bottom
     */
    bool isScrolledToBottom() const;
    
    /**
     * Returns true if the window is scrolled to the bottom
     */
    bool isScrolledToTop() const;
    
protected:
    void _draw_Impl() override;
    
public:
    std::string name;
    
    Vector2F minSize = {0.f, 0.f};
    Vector2F maxSize = {0.f, 0.f};
    
    bool resizable = true;
    bool closable = false;
    bool movable = true;
    bool scrollable = true;
    bool dockable = false;
    bool hideBackground = false;
    bool forceHorizontalScrollbar = false;
    bool forceVerticalScrollbar = false;
    bool allowHorizontalScrollbar = false;
    bool bringToFrontOnFocus = true;
    bool collapsable = false;
    bool allowInputs = true;
    bool titleBar = true;
    
    Event<> openEvent;
    Event<> closeEvent;
    
private:
    bool _opened;
    bool _hovered;
    bool _focused;
    bool _mustScrollToBottom = false;
    bool _mustScrollToTop = false;
    bool _scrolledToBottom = false;
    bool _scrolledToTop = false;
};

}
}
#endif /* panel_window_hpp */
