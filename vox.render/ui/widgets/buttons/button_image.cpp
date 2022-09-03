//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/buttons/button_image.h"

#include <utility>

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
ButtonImage::ButtonImage(wgpu::TextureView p_textureID, const Vector2F &p_size)
    : textureID{std::move(p_textureID)}, size_(p_size) {}
void ButtonImage::DrawImpl() {
    ImVec4 bg = Converter::ToImVec4(background_);
    ImVec4 tn = Converter::ToImVec4(tint_);

    if (ImGui::ImageButton((ImTextureID) textureID.Get(), Converter::ToImVec2(size_),
                           ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), -1, bg, tn))
        clicked_event_.Invoke();
}

}  // namespace vox::ui
