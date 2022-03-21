//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef widget_container_hpp
#define widget_container_hpp

#include "widget.h"

namespace vox {
namespace ui {
/**
 * Defines how the memory should be managed
 */
enum class MemoryMode {
    INTERNAL_MANAGEMENT,
    EXTERNAL_MANAGEMENT
};

/**
 * The base class for any widget container
 */
class WidgetContainer {
public:
    /**
     * Remove a widget from the container
     * @param p_widget p_widget
     */
    void removeWidget(Widget &p_widget);
    
    /**
     * Remove all widgets from the container
     */
    void removeAllWidgets();
    
    /**
     * Consider a widget
     * @param p_manageMemory p_manageMemory
     */
    void considerWidget(Widget &p_widget, bool p_manageMemory = true);
    
    /**
     * Unconsider a widget
     * @param p_widget p_widget
     */
    void unconsiderWidget(Widget &p_widget);
    
    /**
     * Collect garbages by removing widgets marked as "Destroyed"
     */
    void collectGarbages();
    
    /**
     * Draw every widgets
     */
    void drawWidgets();
    
    /**
     * Allow the user to reverse the draw order of this widget container
     */
    void reverseDrawOrder(bool reversed = true);
    
    /**
     * Create a widget
     * @param p_args p_args
     */
    template<typename T, typename ... Args>
    T &createWidget(Args &&... p_args) {
        _widgets.emplace_back(new T(p_args...), MemoryMode::INTERNAL_MANAGEMENT);
        T &instance = *reinterpret_cast<T *>(_widgets.back().first);
        instance.setParent(this);
        return instance;
    }
    
    /**
     * Returns the widgets and their memory management mode
     */
    std::vector<std::pair<Widget *, MemoryMode>> &widgets();
    
protected:
    std::vector<std::pair<Widget *, MemoryMode>> _widgets{};
    bool _reversedDrawOrder = false;
};


}
}
#endif /* widget_container_hpp */
