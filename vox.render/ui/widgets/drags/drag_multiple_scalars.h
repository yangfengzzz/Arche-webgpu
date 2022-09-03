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
 * Drag widget of multiple generic type
 */
template <typename T, size_t Size>
class DragMultipleScalars : public DataWidget<std::array<T, Size>> {
    static_assert(Size > 1, "Invalid DragMultipleScalars _Size (2 or more required)");

public:
    DragMultipleScalars(
            ImGuiDataType_ data_type, T min, T max, T value, float speed, std::string label, std::string format)
        : DataWidget<std::array<T, Size>>(values_),
          data_type_(data_type),
          min_(min),
          max_(max),
          speed_(speed),
          label_(std::move(label)),
          format_(std::move(format)) {
        values_.fill(value);
    }

protected:
    void DrawImpl() override {
        if (max_ < min_) max_ = min_;

        for (size_t i = 0; i < Size; ++i) {
            if (values_[i] < min_)
                values_[i] = min_;
            else if (values_[i] > max_)
                values_[i] = max_;
        }

        if (ImGui::DragScalarN((label_ + DataWidget<std::array<T, Size>>::widget_id_).c_str(), data_type_,
                               values_.data(), Size, speed_, &min_, &max_, format_.c_str())) {
            value_changed_event_.Invoke(values_);
            DataWidget<std::array<T, Size>>::NotifyChange();
        }
    }

public:
    T min_;
    T max_;
    float speed_;
    std::array<T, Size> values_;
    std::string label_;
    std::string format_;
    Event<std::array<T, Size> &> value_changed_event_;

protected:
    ImGuiDataType_ data_type_;
};

}  // namespace vox::ui
