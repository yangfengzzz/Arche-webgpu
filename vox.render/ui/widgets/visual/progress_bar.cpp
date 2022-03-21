//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "progress_bar.h"
#include "ui/widgets/converter.h"

namespace vox {
namespace ui {
ProgressBar::ProgressBar(float p_fraction, const Vector2F &p_size, const std::string &p_overlay) :
fraction(p_fraction), size(p_size), overlay(p_overlay) {
}

void ProgressBar::_draw_Impl() {
    ImGui::ProgressBar(fraction, Converter::ToImVec2(size), !overlay.empty() ? overlay.c_str() : nullptr);
}

}
}
