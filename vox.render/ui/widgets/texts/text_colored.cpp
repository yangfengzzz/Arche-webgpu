//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/texts/text_colored.h"

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
TextColored::TextColored(const std::string &content, const Color &color) : Text(content), color_(color) {}

void TextColored::DrawImpl() { ImGui::TextColored(Converter::ToImVec4(color_), "%s", content_.c_str()); }

}  // namespace vox::ui
