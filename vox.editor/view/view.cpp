//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/view/view.h"

#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/model_mesh.h"

namespace vox::editor::ui {
View::View(const std::string& p_title,
           bool p_opened,
           const PanelWindowSettings& p_windowSettings,
           RenderContext* renderContext)
    : PanelWindow(p_title, p_opened, p_windowSettings), _renderContext(renderContext) {
    scrollable_ = false;
}

void View::update(float p_deltaTime) {
    auto [winWidth, winHeight] = safeSize();

    if (winWidth > 0) {
        if (!_image) {
            _image = &CreateWidget<::vox::ui::ImageUI>(nullptr, Vector2F{0.f, 0.f});
        }

        _image->size = Vector2F(static_cast<float>(winWidth), static_cast<float>(winHeight));
        if (_createRenderTexture(winWidth * 2, winHeight * 2)) {
            _image->SetTextureView(_texture.CreateView());
        }
    }
}

void View::DrawImpl() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PanelWindow::DrawImpl();
    ImGui::PopStyleVar();
}

std::pair<uint16_t, uint16_t> View::safeSize() const {
    auto result = Size() - Vector2F{0.f, 25.f};  // 25 == title bar height
    return {static_cast<uint16_t>(result.x), static_cast<uint16_t>(result.y)};
}

bool View::_createRenderTexture(uint32_t width, uint32_t height) {
    if (width != _textureDesc.size.width || height != _textureDesc.size.height) {
        _textureDesc.dimension = wgpu::TextureDimension::e2D;
        _textureDesc.size.width = width;
        _textureDesc.size.height = height;
        _textureDesc.size.depthOrArrayLayers = 1;
        _textureDesc.sampleCount = 1;
        _textureDesc.format = _renderContext->drawableTextureFormat();
        _textureDesc.mipLevelCount = 1;
        _textureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
        _texture = _renderContext->device().CreateTexture(&_textureDesc);

        _textureDesc.format = _depthStencilTextureFormat;
        _depthStencilTexture = _renderContext->device().CreateTexture(&_textureDesc).CreateView();

        return true;
    } else {
        return false;
    }
}

}  // namespace vox::editor::ui
