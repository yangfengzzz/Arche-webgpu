//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef panel_hpp
#define panel_hpp

#include <vector>
#include <unordered_map>

#include "widget_container.h"

namespace vox {
namespace ui {
/**
 * A Panel is a component of a canvas. It is a sort of window in the UI
 */
class Panel : public Drawable, public WidgetContainer {
public:
    /**
     * Constructor
     */
    Panel();
    
    /**
     * Draw the panel
     */
    void draw() override;
    
    /**
     * Returns the panel identifier
     */
    const std::string &panelID() const;
    
protected:
    virtual void _draw_Impl() = 0;
    
public:
    bool enabled = true;
    
protected:
    std::string _panelID;
    
private:
    static uint64_t __PANEL_ID_INCREMENT;
};


}
}
#endif /* panel_hpp */
