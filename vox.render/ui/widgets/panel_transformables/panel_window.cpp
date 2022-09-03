//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/panel_transformables/panel_window.h"

#include <imgui_internal.h>

#include <utility>

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
PanelWindow::PanelWindow(std::string name, bool opened, const PanelWindowSettings &panel_settings)
    : name_(std::move(name)),
      resizable_(panel_settings.resizable),
      closable_(panel_settings.closable),
      movable_(panel_settings.movable),
      scrollable_(panel_settings.scrollable),
      dockable_(panel_settings.dockable),
      hide_background_(panel_settings.hide_background),
      force_horizontal_scrollbar_(panel_settings.force_horizontal_scrollbar),
      force_vertical_scrollbar_(panel_settings.force_vertical_scrollbar),
      allow_horizontal_scrollbar_(panel_settings.allow_horizontal_scrollbar),
      bring_to_front_on_focus_(panel_settings.bring_to_front_on_focus),
      collapsable_(panel_settings.collapsable),
      allow_inputs_(panel_settings.allow_inputs),
      opened_(opened) {
    auto_size_ = panel_settings.auto_size;
}

void PanelWindow::Open() {
    if (!opened_) {
        opened_ = true;
        open_event_.Invoke();
    }
}

void PanelWindow::Close() {
    if (opened_) {
        opened_ = false;
        close_event_.Invoke();
    }
}

void PanelWindow::Focus() { ImGui::SetWindowFocus((name_ + panel_id_).c_str()); }

void PanelWindow::SetOpened(bool value) {
    if (value != opened_) {
        opened_ = value;

        if (opened_)
            open_event_.Invoke();
        else
            close_event_.Invoke();
    }
}

bool PanelWindow::IsOpened() const { return opened_; }

bool PanelWindow::IsHovered() const { return hovered_; }

bool PanelWindow::IsFocused() const { return focused_; }

bool PanelWindow::IsAppearing() const {
    if (auto window = ImGui::FindWindowByName((name_ + PanelId()).c_str()); window)
        return window->Appearing;
    else
        return false;
}

void PanelWindow::ScrollToBottom() { must_scroll_to_bottom_ = true; }

void PanelWindow::ScrollToTop() { must_scroll_to_top_ = true; }

bool PanelWindow::IsScrolledToBottom() const { return scrolled_to_bottom_; }

bool PanelWindow::IsScrolledToTop() const { return scrolled_to_top_; }

void PanelWindow::DrawImpl() {
    if (opened_) {
        int window_flags = ImGuiWindowFlags_None;

        if (!resizable_) window_flags |= ImGuiWindowFlags_NoResize;
        if (!movable_) window_flags |= ImGuiWindowFlags_NoMove;
        if (!dockable_) window_flags |= ImGuiWindowFlags_NoDocking;
        if (hide_background_) window_flags |= ImGuiWindowFlags_NoBackground;
        if (force_horizontal_scrollbar_) window_flags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
        if (force_vertical_scrollbar_) window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
        if (allow_horizontal_scrollbar_) window_flags |= ImGuiWindowFlags_HorizontalScrollbar;
        if (!bring_to_front_on_focus_) window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (!collapsable_) window_flags |= ImGuiWindowFlags_NoCollapse;
        if (!allow_inputs_) window_flags |= ImGuiWindowFlags_NoInputs;
        if (!scrollable_) window_flags |= ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
        if (!title_bar_) window_flags |= ImGuiWindowFlags_NoTitleBar;

        ImVec2 min_size_constraint = Converter::ToImVec2(min_size_);
        ImVec2 max_size_constraint = Converter::ToImVec2(max_size_);

        /* Cancel constraint if x or y is <= 0.f */
        if (min_size_constraint.x <= 0.f || min_size_constraint.y <= 0.f) min_size_constraint = {0.0f, 0.0f};

        if (max_size_constraint.x <= 0.f || max_size_constraint.y <= 0.f) max_size_constraint = {10000.f, 10000.f};

        ImGui::SetNextWindowSizeConstraints(min_size_constraint, max_size_constraint);

        if (ImGui::Begin((name_ + panel_id_).c_str(), closable_ ? &opened_ : nullptr, window_flags)) {
            hovered_ = ImGui::IsWindowHovered();
            focused_ = ImGui::IsWindowFocused();

            auto scroll_y = ImGui::GetScrollY();

            scrolled_to_bottom_ = scroll_y == ImGui::GetScrollMaxY();
            scrolled_to_top_ = scroll_y == 0.0f;

            if (!opened_) close_event_.Invoke();

            Update();

            if (must_scroll_to_bottom_) {
                ImGui::SetScrollY(ImGui::GetScrollMaxY());
                must_scroll_to_bottom_ = false;
            }

            if (must_scroll_to_top_) {
                ImGui::SetScrollY(0.0f);
                must_scroll_to_top_ = false;
            }

            DrawWidgets();
        }

        ImGui::End();
    }
}

}  // namespace vox::ui
