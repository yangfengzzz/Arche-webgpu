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
 * Represents a drag and drop target
 */
template <typename T>
class DDTarget : public Plugin {
public:
    /**
     * Create the drag and drop target
     */
    explicit DDTarget(std::string identifier) : identifier_(std::move(identifier)) {}

    /**
     * Execute the drag and drop target behaviour
     */
    void Execute() override {
        if (ImGui::BeginDragDropTarget()) {
            if (!is_hovered_) hover_start_event_.Invoke();

            is_hovered_ = true;

            ImGuiDragDropFlags target_flags = 0;
            // Don't wait until the delivery (release mouse button on a target) to do something
            // target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;

            if (!show_yellow_rect_)
                target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect;  // Don't display the yellow rectangle

            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(identifier_.c_str(), target_flags)) {
                T data = *(T *)payload->Data;
                data_received_event_.Invoke(data);
            }
            ImGui::EndDragDropTarget();
        } else {
            if (is_hovered_) hover_end_event_.Invoke();

            is_hovered_ = false;
        }
    }

    /**
     * Returns true if the drag and drop target is hovered by a drag and drop source
     */
    [[nodiscard]] bool IsHovered() const { return is_hovered_; }

public:
    std::string identifier_;
    Event<T> data_received_event_;
    Event<> hover_start_event_;
    Event<> hover_end_event_;

    bool show_yellow_rect_ = true;

private:
    bool is_hovered_{};
};

}  // namespace vox::ui
