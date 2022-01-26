//
//  render_context.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/25.
//

#include "render_context.h"

namespace vox {
RenderContext::RenderContext(BackendBinding* binding, uint32_t width, uint32_t height):
_binding(binding),
_width(width),
_height(height) {
    wgpu::SwapChainDescriptor swapChainDesc;
    swapChainDesc.implementation = binding->swapChainImplementation();
    _swapchain = _binding->device().CreateSwapChain(nullptr, &swapChainDesc);
    
    _swapchain.Configure(drawableTextureFormat(),
                         wgpu::TextureUsage::RenderAttachment, _width, _height);
    _depthStencilTexture = _createDepthStencilView(_width, _height);
}

wgpu::Device& RenderContext::device() {
    return _binding->device();
}

wgpu::TextureView RenderContext::currentDrawableTexture() {
    return _swapchain.GetCurrentTextureView();
}

wgpu::TextureView RenderContext::depthStencilTexture() {
    return _depthStencilTexture;
}

void RenderContext::resize(uint32_t width, uint32_t height) {
    if (width != _width || height != _height) {
        _swapchain.Configure(drawableTextureFormat(),
                             wgpu::TextureUsage::RenderAttachment, width, height);
        _createDepthStencilView(width, height);
    }
    _width = width;
    _height = height;
}

wgpu::TextureFormat RenderContext::drawableTextureFormat() {
    return _binding->preferredSwapChainTextureFormat();
}

wgpu::TextureFormat RenderContext::depthStencilTextureFormat() {
    return _depthStencilTextureFormat;
}

void RenderContext::present() {
    _swapchain.Present();
}

wgpu::TextureView RenderContext::_createDepthStencilView(uint32_t width, uint32_t height) {
    wgpu::TextureDescriptor descriptor;
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.size.width = width;
    descriptor.size.height = height;
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = _depthStencilTextureFormat;
    descriptor.mipLevelCount = 1;
    descriptor.usage = wgpu::TextureUsage::RenderAttachment;
    auto depthStencilTexture = _binding->device().CreateTexture(&descriptor);
    return depthStencilTexture.CreateView();
}

}
