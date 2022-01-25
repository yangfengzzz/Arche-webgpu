//
//  render_context.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/25.
//

#include "render_context.h"

namespace vox {
RenderContext::RenderContext(wgpu::Device& device, uint32_t width, uint32_t height):
_device(device) {
    
    _swapchain.Configure(DRAWBLE_TEXTURE_FORMAT, wgpu::TextureUsage::RenderAttachment, _width, _height);

    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.width = width;
    descriptor.size.height = height;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = DEPTH_STENCIL_TEXTURE_FORMAT;
    descriptor.mipLevelCount = 1;
    descriptor.usage = wgpu::TextureUsage::RenderAttachment;
    auto depthStencilTexture = device.CreateTexture(&descriptor);
    _depthStencilTexture = depthStencilTexture.CreateView();
}

wgpu::Device &RenderContext::device() {
    return _device;
}

wgpu::TextureView RenderContext::currentDrawableTexture() {
    if (_isDirty) {
        _swapchain.Configure(DRAWBLE_TEXTURE_FORMAT, wgpu::TextureUsage::RenderAttachment, _width, _height);
    }
    return _swapchain.GetCurrentTextureView();
}

wgpu::TextureView RenderContext::depthStencilTexture() {
    return _depthStencilTexture;
}

void RenderContext::resize(uint32_t width, uint32_t height) {
    if (width != _width || height != _height) {
        _isDirty = true;
    }
    _width = width;
    _height = height;
}

}
