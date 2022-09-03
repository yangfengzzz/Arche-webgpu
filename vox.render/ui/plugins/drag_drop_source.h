//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <imgui.h>

#include <string>
#include <utility>

#include "vox.render/event.h"
#include "vox.render/ui/plugins/plugin.h"

namespace vox::ui {
/**
 * Represents a drag and drop source
 */
template <typename T>
class DDSource : public Plugin {
public:
    /**
     * Create the drag and drop source
     */
    DDSource(std::string identifier, std::string tooltip, T data)
        : identifier_(std::move(identifier)), tooltip_(std::move(tooltip)), data_(data) {}

    /**
     * Execute the behaviour of the drag and drop source
     */
    void Execute() override {
        ImGuiDragDropFlags src_flags = 0;
        // Keep the source displayed as hovered
        src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;
        // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
        src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;

        if (!has_tooltip_) src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip;  // Hide the tooltip

        if (ImGui::BeginDragDropSource(src_flags)) {
            if (!is_dragged_) drag_start_event_.Invoke();

            is_dragged_ = true;

            if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip)) ImGui::Text("%s", tooltip_.c_str());
            ImGui::SetDragDropPayload(identifier_.c_str(), &data_, sizeof(data_));
            ImGui::EndDragDropSource();
        } else {
            if (is_dragged_) drag_stop_event_.Invoke();

            is_dragged_ = false;
        }
    }

    /**
     * Returns true if the drag and drop source is dragged
     */
    [[nodiscard]] bool IsDragged() const { return is_dragged_; }

public:
    std::string identifier_;
    std::string tooltip_;
    T data_;
    Event<> drag_start_event_;
    Event<> drag_stop_event_;

    bool has_tooltip_ = true;

private:
    bool is_dragged_{};
};

}  // namespace vox::ui
