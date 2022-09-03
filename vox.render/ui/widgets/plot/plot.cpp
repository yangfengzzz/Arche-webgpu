//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/plot/plot.h"

#include <utility>

namespace vox::ui {
Plot::Plot(std::vector<float> data,
           float min_scale,
           float max_scale,
           const Vector2F &size,
           std::string overlay,
           std::string label)
    : DataWidget(data_),
      data_(std::move(data)),
      min_scale_(min_scale),
      max_scale_(max_scale),
      size_(size),
      overlay_(std::move(overlay)),
      label_(std::move(label)) {}

}  // namespace vox::ui
