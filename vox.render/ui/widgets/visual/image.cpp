//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/visual/image.h"

#include <utility>

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
Image::Image(wgpu::TextureView p_textureID, const Vector2F &p_size) : textureID{std::move(p_textureID)}, size(p_size) {}

void Image::SetTextureView(wgpu::TextureView p_textureID) { textureID = std::move(p_textureID); }

void Image::DrawImpl() { ImGui::Image(textureID.Get(), Converter::ToImVec2(size), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f)); }
}  // namespace vox::ui
