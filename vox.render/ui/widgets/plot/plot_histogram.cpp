//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "plot_histogram.h"
#include "ui/widgets/converter.h"

namespace vox {
namespace ui {
PlotHistogram::PlotHistogram(const std::vector<float> &p_data,
                             float p_minScale,
                             float p_maxScale,
                             const Vector2F &p_size,
                             const std::string &p_overlay,
                             const std::string &p_label) :
Plot(p_data, p_minScale, p_maxScale, p_size, p_overlay, p_label) {
}

void PlotHistogram::_draw_Impl() {
    ImGui::PlotHistogram((label + _widgetID).c_str(), data.data(), static_cast<int>(data.size()), 0,
                         overlay.c_str(), minScale, maxScale, Converter::ToImVec2(size), sizeof(float));
}

}
}
