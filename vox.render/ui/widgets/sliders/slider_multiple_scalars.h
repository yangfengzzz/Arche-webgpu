//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>
#include <utility>

#include "vox.render/event.h"
#include "vox.render/ui/widgets/data_widget.h"

namespace vox::ui {
/**
 * Slider widget of multiple generic type
 */
template <typename T, size_t Size>
class SliderMultipleScalars : public DataWidget<std::array<T, Size>> {
    using DataWidget<std::array<T, Size>>::widget_id_;
    using DataWidget<std::array<T, Size>>::NotifyChange;

    static_assert(Size > 1, "Invalid SliderMultipleScalars _Size (2 or more required)");

public:
    SliderMultipleScalars(ImGuiDataType_ data_type, T min, T max, T value, std::string label, std::string format)
        : DataWidget<std::array<T, Size>>(values_),
          data_type_(data_type),
          min_(min),
          max_(max),
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

        bool value_changed = ImGui::SliderScalarN((label_ + widget_id_).c_str(), data_type_, values_.data(), Size,
                                                  &min_, &max_, format_.c_str());

        if (value_changed) {
            value_changed_event_.Invoke(values_);
            NotifyChange();
        }
    }

public:
    T min_;
    T max_;
    std::array<T, Size> values_;
    std::string label_;
    std::string format_;
    Event<std::array<T, Size> &> value_changed_event_;

protected:
    ImGuiDataType_ data_type_;
};

}  // namespace vox::ui
