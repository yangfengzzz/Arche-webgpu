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
 * Input widget of generic type
 */
template <typename T>
class InputSingleScalar : public DataWidget<T> {
    static_assert(std::is_scalar<T>::value, "Invalid InputSingleScalar T (Scalar expected)");

public:
    InputSingleScalar(ImGuiDataType data_type,
                      T default_value,
                      T step,
                      T fast_step,
                      std::string label,
                      std::string format,
                      bool select_all_on_click)
        : DataWidget<T>(value_),
          data_type_(data_type),
          value_(default_value),
          step_(step),
          fast_step_(fast_step),
          label_(std::move(label)),
          format_(std::move(format)),
          select_all_on_click_(select_all_on_click) {}

protected:
    void DrawImpl() override {
        T previous_value = value_;

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (select_all_on_click_) flags |= ImGuiInputTextFlags_AutoSelectAll;

        bool enter_pressed = ImGui::InputScalar((label_ + DataWidget<T>::widget_id_).c_str(), data_type_, &value_,
                                                step_ != 0.0f ? &step_ : nullptr,
                                                fast_step_ != 0.0f ? &fast_step_ : nullptr, format_.c_str(), flags);

        if (previous_value != value_) {
            content_changed_event_.Invoke(value_);
            DataWidget<T>::NotifyChange();
        }

        if (enter_pressed) enter_pressed_event_.Invoke(value_);
    }

public:
    T value_;
    T step_;
    T fast_step_;
    std::string label_;
    std::string format_;
    bool select_all_on_click_;
    Event<T> content_changed_event_;
    Event<T> enter_pressed_event_;

private:
    ImGuiDataType data_type_;
};

}  // namespace vox::ui
