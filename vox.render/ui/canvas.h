//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef canvas_hpp
#define canvas_hpp

#include <algorithm>

#include "event.h"

#include "ui/drawable.h"
#include "ui/widgets/panel.h"

namespace vox {
namespace ui {
/**
 * A Canvas represents the whole frame available for UI drawing
 */
class Canvas : public Drawable {
public:
    /**
     * Adds a panel to the canvas
     */
    void addPanel(Panel &p_panel);
    
    /**
     * Removes a panel from the canvas
     */
    void removePanel(Panel &p_panel);
    
    /**
     * Removes every panels from the canvas
     */
    void removeAllPanels();
    
    /**
     * Makes the canvas a dockspace (Dockable panels will be able to attach themselves to the canvas)
     */
    void makeDockspace(bool p_state);
    
    /**
     * Returns true if the canvas is a dockspace
     */
    bool isDockspace() const;
    
    /**
     * Draw the canvas (Draw every panels)
     */
    void draw() override;
    
private:
    std::vector<std::reference_wrapper<Panel>> _panels;
    bool _isDockspace = false;
};


}
}
#endif /* canvas_hpp */
