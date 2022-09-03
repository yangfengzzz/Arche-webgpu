//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <vector>

#include "vox.math/vector2.h"
#include "vox.render/event.h"
#include "vox.render/ui/widgets/alignment.h"
#include "vox.render/ui/widgets/panel.h"

namespace vox::ui {
/**
 * APanelTransformable is a panel that is localized in the canvas
 */
class PanelTransformable : public Panel {
public:
    explicit PanelTransformable(const Vector2F &default_position = Vector2F(-1.f, -1.f),
                                const Vector2F &default_size = Vector2F(-1.f, -1.f),
                                HorizontalAlignment default_horizontal_alignment = HorizontalAlignment::LEFT,
                                VerticalAlignment default_vertical_alignment = VerticalAlignment::TOP,
                                bool ignore_config_file = false);

    /**
     * Defines the position of the panel
     */
    void SetPosition(const Vector2F &position);

    /**
     * Defines the size of the panel
     */
    void SetSize(const Vector2F &size);

    /**
     * Defines the alignment of the panel
     */
    void SetAlignment(HorizontalAlignment horizontal_alignment, VerticalAlignment vertical_alignment);

    /**
     * Returns the current position of the panel
     */
    [[nodiscard]] const Vector2F &Position() const;

    /**
     * Returns the current size of the panel
     */
    [[nodiscard]] const Vector2F &Size() const;

    /**
     * Returns the current horizontal alignment of the panel
     */
    [[nodiscard]] HorizontalAlignment GetHorizontalAlignment() const;

    /**
     * Returns the current vertical alignment of the panel
     */
    [[nodiscard]] VerticalAlignment GetVerticalAlignment() const;

protected:
    void Update();

    void DrawImpl() override = 0;

private:
    Vector2F CalculatePositionAlignmentOffset(bool is_default = false);

    void UpdatePosition();

    void UpdateSize();

    void CopyImGuiPosition();

    void CopyImGuiSize();

public:
    bool auto_size_ = true;

protected:
    Vector2F default_position_;
    Vector2F default_size_;
    HorizontalAlignment default_horizontal_alignment_;
    VerticalAlignment default_vertical_alignment_;
    bool ignore_config_file_;

    Vector2F position_ = Vector2F(0.0f, 0.0f);
    Vector2F size_ = Vector2F(0.0f, 0.0f);

    bool position_changed_ = false;
    bool size_changed_ = false;

    HorizontalAlignment horizontal_alignment_ = HorizontalAlignment::LEFT;
    VerticalAlignment vertical_alignment_ = VerticalAlignment::TOP;

    bool alignment_changed_ = false;
    bool first_frame_ = true;
};

}  // namespace vox::ui
