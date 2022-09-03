//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.math/vector2.h"
#include "vox.render/event.h"
#include "vox.render/ui/widgets/converter.h"
#include "vox.render/ui/widgets/data_widget.h"

namespace vox::ui {
/**
 * Defines the slider orientation
 */
enum class SliderOrientation { HORIZONTAL, VERTICAL };

/**
 * Slider widget of generic type
 */
template <typename T>
class SliderSingleScalar : public DataWidget<T> {
    static_assert(std::is_scalar<T>::value, "Invalid SliderSingleScalar T (Scalar expected)");

public:
    SliderSingleScalar(ImGuiDataType data_type,
                       T min,
                       T max,
                       T value,
                       SliderOrientation orientation,
                       std::string label,
                       std::string format)
        : DataWidget<T>(value_),
          data_type_(data_type),
          min_(min),
          max_(max),
          value_(value),
          orientation_(orientation),
          label_(std::move(label)),
          format_(std::move(format)) {}

protected:
    void DrawImpl() override {
        if (max_ < min_) max_ = min_;

        if (value_ < min_)
            value_ = min_;
        else if (value_ > max_)
            value_ = max_;

        bool value_changed = false;

        switch (orientation_) {
            case SliderOrientation::HORIZONTAL:
                value_changed = ImGui::SliderScalar((label_ + DataWidget<T>::widget_id_).c_str(), data_type_, &value_,
                                                    &min_, &max_, format_.c_str());
                break;
            case SliderOrientation::VERTICAL:
                value_changed = ImGui::VSliderScalar((label_ + DataWidget<T>::widget_id_).c_str(),
                                                     Converter::ToImVec2(vertical_mode_size_), data_type_, &value_,
                                                     &min_, &max_, format_.c_str());
                break;
        }

        if (value_changed) {
            value_changed_event_.Invoke(value_);
            DataWidget<T>::NotifyChange();
        }
    }

public:
    T min_;
    T max_;
    T value_;
    SliderOrientation orientation_;
    Vector2F vertical_mode_size_; /* Only applied with SliderOrientation::VERTICAL */
    std::string label_;
    std::string format_;
    Event<T> value_changed_event_;

private:
    ImGuiDataType data_type_;
};

}  // namespace vox::ui
