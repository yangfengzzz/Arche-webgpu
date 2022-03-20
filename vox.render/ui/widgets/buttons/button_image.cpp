//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "button_image.h"
#include "ui/widgets/converter.h"
#include "gui/imgui_internal.h"

namespace vox {
namespace ui {
ButtonImage::ButtonImage(wgpu::TextureView p_textureID, const Vector2F &p_size) :
textureID{p_textureID}, size(p_size) {
}

void ButtonImage::_draw_Impl() {
    ImVec4 bg = Converter::ToImVec4(background);
    ImVec4 tn = Converter::ToImVec4(tint);
    
    if (ImGui::ImageButton((ImTextureID) textureID.Get(), Converter::ToImVec2(size),
                           ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), -1, bg, tn))
        clickedEvent.invoke();
}

}
}
