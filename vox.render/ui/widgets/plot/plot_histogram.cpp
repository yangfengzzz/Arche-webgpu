//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/plot/plot_histogram.h"

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
PlotHistogram::PlotHistogram(const std::vector<float> &data,
                             float min_scale,
                             float max_scale,
                             const Vector2F &size,
                             const std::string &overlay,
                             const std::string &label)
    : Plot(data, min_scale, max_scale, size, overlay, label) {}

void PlotHistogram::DrawImpl() {
    ImGui::PlotHistogram((label_ + widget_id_).c_str(), data_.data(), static_cast<int>(data_.size()), 0,
                         overlay_.c_str(), min_scale_, max_scale_, Converter::ToImVec2(size_), sizeof(float));
}

}  // namespace vox::ui
