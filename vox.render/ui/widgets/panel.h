//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_map>
#include <vector>

#include "vox.render/ui/widgets/widget_container.h"

namespace vox::ui {
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
    void Draw() override;

    /**
     * Returns the panel identifier
     */
    [[nodiscard]] const std::string &PanelId() const;

protected:
    virtual void DrawImpl() = 0;

public:
    bool enabled_ = true;

protected:
    std::string panel_id_;

private:
    static uint64_t panel_id_increment_;
};

}  // namespace vox::ui
