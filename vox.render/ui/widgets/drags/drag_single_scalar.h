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
 * Drag widget of generic type
 */
template <typename T>
class DragSingleScalar : public DataWidget<T> {
    static_assert(std::is_scalar<T>::value, "Invalid DragSingleScalar T (Scalar expected)");

public:
    DragSingleScalar(ImGuiDataType data_type, T min, T max, T value, float speed, std::string label, std::string format)
        : DataWidget<T>(value_),
          data_type_(data_type),
          min_(min),
          max_(max),
          value_(value),
          speed_(speed),
          label_(std::move(label)),
          format_(std::move(format)) {}

protected:
    void DrawImpl() override {
        if (max_ < min_) max_ = min_;

        if (value_ < min_)
            value_ = min_;
        else if (value_ > max_)
            value_ = max_;

        if (ImGui::DragScalar((label_ + DataWidget<T>::widget_id_).c_str(), data_type_, &value_, speed_, &min_, &max_,
                              format_.c_str())) {
            value_changed_event_.Invoke(value_);
            DataWidget<T>::NotifyChange();
        }
    }

public:
    T min_;
    T max_;
    T value_;
    float speed_;
    std::string label_;
    std::string format_;
    Event<T> value_changed_event_;

private:
    ImGuiDataType data_type_;
};

}  // namespace vox::ui
