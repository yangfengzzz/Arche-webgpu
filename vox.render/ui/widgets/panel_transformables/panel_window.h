//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/event.h"
#include "vox.render/ui/widgets/panel_transformable.h"
#include "vox.render/ui/widgets/panel_window_settings.h"

namespace vox::ui {
/**
 * A PanelWindow is a panel that is localized and behave like a window (Movable, resizable...)
 */
class PanelWindow : public PanelTransformable {
public:
    explicit PanelWindow(std::string name = "",
                         bool opened = true,
                         const PanelWindowSettings &panel_settings = PanelWindowSettings{});

    /**
     * Open (show) the panel
     */
    void Open();

    /**
     * Close (hide) the panel
     */
    void Close();

    /**
     * Focus the panel
     */
    void Focus();

    /**
     * Defines the opened state of the window
     */
    void SetOpened(bool value);

    /**
     * Returns true if the panel is opened
     */
    [[nodiscard]] bool IsOpened() const;

    /**
     * Returns true if the panel is hovered
     */
    [[nodiscard]] bool IsHovered() const;

    /**
     * Returns true if the panel is focused
     */
    [[nodiscard]] bool IsFocused() const;

    /**
     * Returns true if the panel is appearing
     */
    [[nodiscard]] bool IsAppearing() const;

    /**
     * Scrolls to the bottom of the window
     */
    void ScrollToBottom();

    /**
     * Scrolls to the top of the window
     */
    void ScrollToTop();

    /**
     * Returns true if the window is scrolled to the bottom
     */
    [[nodiscard]] bool IsScrolledToBottom() const;

    /**
     * Returns true if the window is scrolled to the bottom
     */
    [[nodiscard]] bool IsScrolledToTop() const;

protected:
    void DrawImpl() override;

public:
    std::string name_;

    Vector2F min_size_ = {0.f, 0.f};
    Vector2F max_size_ = {0.f, 0.f};

    bool resizable_ = true;
    bool closable_ = false;
    bool movable_ = true;
    bool scrollable_ = true;
    bool dockable_ = false;
    bool hide_background_ = false;
    bool force_horizontal_scrollbar_ = false;
    bool force_vertical_scrollbar_ = false;
    bool allow_horizontal_scrollbar_ = false;
    bool bring_to_front_on_focus_ = true;
    bool collapsable_ = false;
    bool allow_inputs_ = true;
    bool title_bar_ = true;

    Event<> open_event_;
    Event<> close_event_;

private:
    bool opened_;
    bool hovered_{};
    bool focused_{};
    bool must_scroll_to_bottom_ = false;
    bool must_scroll_to_top_ = false;
    bool scrolled_to_bottom_ = false;
    bool scrolled_to_top_ = false;
};

}  // namespace vox::ui
