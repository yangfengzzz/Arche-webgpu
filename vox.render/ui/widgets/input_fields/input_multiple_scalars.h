//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.render/event.h"
#include "vox.render/ui/widgets/data_widget.h"

namespace vox::ui {
/**
 * Input widget of multiple generic type
 */
template <typename T, size_t Size>
class InputMultipleScalars : public DataWidget<std::array<T, Size>> {
    using DataWidget<std::array<T, Size>>::widget_id_;

    static_assert(Size > 1, "Invalid InputMultipleScalars _Size (2 or more required)");
    static_assert(std::is_scalar<T>::value, "Invalid InputMultipleScalars T (Scalar expected)");

public:
    InputMultipleScalars(ImGuiDataType data_type,
                         T default_value,
                         T step,
                         T fast_step,
                         std::string label,
                         std::string format,
                         bool select_all_on_click)
        : DataWidget<std::array<T, Size>>(values_),
          data_type_(data_type),
          step_(step),
          fast_step_(fast_step),
          label_(std::move(label)),
          format_(std::move(format)),
          select_all_on_click_(select_all_on_click) {
        values_.fill(default_value);
    }

protected:
    void DrawImpl() override {
        std::array<T, Size> previous_value = values_;

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (select_all_on_click_) flags |= ImGuiInputTextFlags_AutoSelectAll;

        bool enter_pressed = ImGui::InputScalarN((label_ + widget_id_).c_str(), data_type_, values_.data(), Size,
                                                 step_ != 0.0f ? &step_ : nullptr,
                                                 fast_step_ != 0.0f ? &fast_step_ : nullptr, format_.c_str(), flags);

        bool has_changed = false;

        for (size_t i = 0; i < Size; ++i)
            if (previous_value[i] != values_[i]) has_changed = true;

        if (has_changed) {
            content_changed_event_.Invoke(values_);
            this->NotifyChange();
        }

        if (enter_pressed) enter_pressed_event_.Invoke(values_);
    }

public:
    std::array<T, Size> values_;
    T step_;
    T fast_step_;
    std::string label_;
    std::string format_;
    bool select_all_on_click_;
    Event<std::array<T, Size> &> content_changed_event_;
    Event<std::array<T, Size> &> enter_pressed_event_;

private:
    ImGuiDataType data_type_;
};

}  // namespace vox::ui
