//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <limits>
#include <vector>

#include "vox.math/vector2.h"
#include "vox.render/ui/widgets/data_widget.h"
#include "vox.render/ui/widgets/widget.h"

namespace vox::ui {
/**
 * Base class for any plot widget
 */
class Plot : public DataWidget<std::vector<float>> {
public:
    explicit Plot(std::vector<float> data = std::vector<float>(),
                  float min_scale = std::numeric_limits<float>::min(),
                  float max_scale = std::numeric_limits<float>::max(),
                  const Vector2F &size = {0.0f, 0.0f},
                  std::string overlay = "",
                  std::string label = "");

protected:
    void DrawImpl() override = 0;

public:
    std::vector<float> data_;
    float min_scale_;
    float max_scale_;
    Vector2F size_;
    std::string overlay_;
    std::string label_;
};

}  // namespace vox::ui
