//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/layout/dummy.h"

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
Dummy::Dummy(const Vector2F &size) : size_(size) {}

void Dummy::DrawImpl() { ImGui::Dummy(Converter::ToImVec2(size_)); }

}  // namespace vox::ui
