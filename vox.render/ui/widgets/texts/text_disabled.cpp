//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/texts/text_disabled.h"

namespace vox::ui {
TextDisabled::TextDisabled(const std::string &content) : Text(content) {}

void TextDisabled::DrawImpl() { ImGui::TextDisabled("%s", content_.c_str()); }

}  // namespace vox::ui
