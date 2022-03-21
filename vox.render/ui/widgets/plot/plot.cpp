//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "plot.h"

namespace vox {
namespace ui {
Plot::Plot(const std::vector<float> &p_data,
           float p_minScale,
           float p_maxScale,
           const Vector2F &p_size,
           const std::string &p_overlay,
           const std::string &p_label) :
DataWidget(data), data(p_data), minScale(p_minScale), maxScale(p_maxScale),
size(p_size), overlay(p_overlay), label(p_label) {
}

}
}
