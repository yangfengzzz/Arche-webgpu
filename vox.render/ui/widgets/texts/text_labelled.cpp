//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/texts/text_labelled.h"

#include <utility>

namespace vox::ui {
TextLabelled::TextLabelled(const std::string &content, std::string label) : Text(content), label_(std::move(label)) {}

void TextLabelled::DrawImpl() { ImGui::LabelText((label_ + widget_id_).c_str(), "%s", content_.c_str()); }

}  // namespace vox::ui
