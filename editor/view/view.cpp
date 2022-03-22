//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "view.h"

namespace vox {
namespace editor {
namespace ui {
View::View(wgpu::Device &device,
           const std::string& p_title,
           bool p_opened,
           const PanelWindowSettings& p_windowSettings) :
PanelWindow(p_title, p_opened, p_windowSettings),
_device(device) {
    _image = &createWidget<Image>(_texture.CreateView().Get(), Vector2F{ 0.f, 0.f });
    
    scrollable = false;
}

void View::update(float p_deltaTime) {
    auto[winWidth, winHeight] = safeSize();
    
    _image->size = Vector2F(static_cast<float>(winWidth), static_cast<float>(winHeight));
    
    _textureDesc.size.width = winWidth;
    _textureDesc.size.height = winHeight;
    _texture = _device.CreateTexture(&_textureDesc);
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

}
}
}
