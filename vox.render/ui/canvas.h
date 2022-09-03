//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>

#include "vox.render/event.h"
#include "vox.render/ui/drawable.h"
#include "vox.render/ui/widgets/panel.h"

namespace vox::ui {
/**
 * A Canvas represents the whole frame available for UI drawing
 */
class Canvas : public Drawable {
public:
    /**
     * Adds a panel to the canvas
     */
    void AddPanel(Panel &panel);

    /**
     * Removes a panel from the canvas
     */
    void RemovePanel(Panel &panel);

    /**
     * Removes every panels from the canvas
     */
    void RemoveAllPanels();

    /**
     * Makes the canvas a dockspace (Dockable panels will be able to attach themselves to the canvas)
     */
    void MakeDockSpace(bool state);

    /**
     * Returns true if the canvas is a dockspace
     */
    [[nodiscard]] bool IsDockSpace() const;

    /**
     * Draw the canvas (Draw every panels)
     */
    void Draw() override;

private:
    std::vector<std::reference_wrapper<Panel>> panels_;
    bool is_dockspace_ = false;
};

}  // namespace vox::ui
