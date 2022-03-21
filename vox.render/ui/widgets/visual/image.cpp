//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "image.h"
#include "ui/widgets/converter.h"

namespace vox {
namespace ui {
Image::Image(wgpu::TextureView p_textureID, const Vector2F &p_size) :
textureID{p_textureID}, size(p_size) {
    
}

void Image::_draw_Impl() {
    ImGui::Image(textureID.Get(), Converter::ToImVec2(size),
                 ImVec2(0.f, 0.f), ImVec2(1.f, 1.f));
}

}
}
