//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/visual/progress_bar.h"

#include <utility>

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
ProgressBar::ProgressBar(float fraction, const Vector2F &size, std::string overlay)
    : fraction_(fraction), size_(size), overlay_(std::move(overlay)) {}

void ProgressBar::DrawImpl() {
    ImGui::ProgressBar(fraction_, Converter::ToImVec2(size_), !overlay_.empty() ? overlay_.c_str() : nullptr);
}

}  // namespace vox::ui
