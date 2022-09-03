//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/plot/plot.h"

namespace vox::ui {
/**
 * Plot displayed as lines
 */
class PlotLines : public Plot {
public:
    explicit PlotLines(const std::vector<float> &data = std::vector<float>(),
                       float min_scale = std::numeric_limits<float>::min(),
                       float max_scale = std::numeric_limits<float>::max(),
                       const Vector2F &size = {0.0f, 0.0f},
                       const std::string &overlay = "",
                       const std::string &label = "");

protected:
    void DrawImpl() override;
};

}  // namespace vox::ui
