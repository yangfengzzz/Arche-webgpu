//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "dummy.h"
#include "ui/widgets/converter.h"

namespace vox {
namespace ui {
Dummy::Dummy(const Vector2F &p_size) : size(p_size) {
}

void Dummy::_draw_Impl() {
    ImGui::Dummy(Converter::ToImVec2(size));
}

}
}
