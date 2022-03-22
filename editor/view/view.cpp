//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "view.h"

namespace vox {
namespace editor {
namespace ui {
View::View(RenderContext* renderContext,
           const std::string& p_title,
           bool p_opened,
           const PanelWindowSettings& p_windowSettings) :
PanelWindow(p_title, p_opened, p_windowSettings),
_renderContext(renderContext) {
    scrollable = false;
}

void View::update(float p_deltaTime) {
    auto[winWidth, winHeight] = safeSize();
    
    if (!_image) {
        _image = &createWidget<Image>(_texture.CreateView(), Vector2F{ 0.f, 0.f });
    }
    
    _image->size = Vector2F(static_cast<float>(winWidth), static_cast<float>(winHeight));
    if (_createRenderTexture(winWidth, winHeight)) {
        _image->setTextureView(_texture.CreateView());
    }
}

void View::_draw_Impl() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PanelWindow::_draw_Impl();
    ImGui::PopStyleVar();
}

std::pair<uint16_t, uint16_t> View::safeSize() const {
    auto result = size() - Vector2F{ 0.f, 25.f }; // 25 == title bar height
    return { static_cast<uint16_t>(result.x), static_cast<uint16_t>(result.y) };
}

const Vector3F& View::gridColor() const {
    return _gridColor;
}

void View::setGridColor(const Vector3F& p_color) {
    _gridColor = p_color;
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
        _textureDesc.usage = wgpu::TextureUsage::RenderAttachment;
        _texture = _renderContext->device().CreateTexture(&_textureDesc);
        
        _textureDesc.format = _depthStencilTextureFormat;
        _depthStencilTexture = _renderContext->device().CreateTexture(&_textureDesc).CreateView();
        
        return true;
    } else {
        return false;
    }
}

}
}
}
