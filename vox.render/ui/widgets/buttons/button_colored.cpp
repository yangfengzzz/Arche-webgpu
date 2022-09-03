//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/buttons/button_colored.h"

#include <utility>

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
ButtonColored::ButtonColored(std::string label, const Color &color, const Vector2F &size, bool enable_alpha)
    : label_(std::move(label)), color_(color), size_(size), enable_alpha_(enable_alpha) {}

void ButtonColored::DrawImpl() {
    ImVec4 imColor = Converter::ToImVec4(color_);

    if (ImGui::ColorButton((label_ + widget_id_).c_str(), imColor, !enable_alpha_ ? ImGuiColorEditFlags_NoAlpha : 0,
                           Converter::ToImVec2(size_)))
        clicked_event_.Invoke();

    color_ = Converter::ToColor(imColor);
}

}  // namespace vox::ui
