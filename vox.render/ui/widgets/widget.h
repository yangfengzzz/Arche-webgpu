//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef widget_hpp
#define widget_hpp

#include <string>

#include "ui/drawable.h"
#include "ui/plugins/pluginable.h"
#include "ui/plugins/data_dispatcher.h"
#include "gui/imgui.h"

namespace vox {
namespace ui {
class WidgetContainer;

/**
 * A AWidget is the base class for any widget of UI.
 * It is basically a visual element that can be placed into a panel.
 * It is drawable and can receive plugins
 */
class Widget : public Drawable, public Pluginable {
public:
    /**
     * Constructor
     */
    Widget();
    
    /**
     * Draw the widget on the panel
     */
    virtual void draw() override;
    
    /**
     * Link the widget to another one by making its id identical to the given widget
     * @param p_widget p_widget
     */
    void linkTo(const Widget &p_widget);
    
    /**
     * Mark the widget as destroyed. It will be removed from the owner panel
     * by the garbage collector
     */
    void destroy();
    
    /**
     * Returns true if the widget is marked as destroyed
     */
    bool isDestroyed() const;
    
    /**
     * Defines the parent of this widget
     * @param p_parent p_parent
     */
    void setParent(WidgetContainer *p_parent);
    
    /**
     * Returns true if the widget has a parent
     */
    bool hasParent() const;
    
    /**
     * Returns the parent of the widget or nullptr if there is no parent
     */
    WidgetContainer *parent();
    
protected:
    virtual void _draw_Impl() = 0;
    
public:
    bool enabled = true;
    bool lineBreak = true;
    
protected:
    WidgetContainer *_parent{nullptr};
    std::string _widgetID = "?";
    bool _autoExecutePlugins = true;
    
private:
    static uint64_t __WIDGET_ID_INCREMENT;
    bool _destroyed = false;
};


}
}
#endif /* widget_hpp */
