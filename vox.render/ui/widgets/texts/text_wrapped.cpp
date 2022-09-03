//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/texts/text_wrapped.h"

namespace vox::ui {
TextWrapped::TextWrapped(const std::string &content) : Text(content) {}

void TextWrapped::DrawImpl() { ImGui::TextWrapped("%s", content_.c_str()); }

}  // namespace vox::ui
