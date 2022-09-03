//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/render_context.h"

namespace vox {
RenderContext::RenderContext(BackendBinding* binding, uint32_t width, uint32_t height)
    : _binding(binding), _width(width), _height(height) {
    wgpu::SwapChainDescriptor swapChainDesc;
    swapChainDesc.implementation = binding->swapChainImplementation();
    _swapchain = _binding->device().CreateSwapChain(nullptr, &swapChainDesc);

    _swapchain.Configure(drawableTextureFormat(), wgpu::TextureUsage::RenderAttachment, _width, _height);
}

wgpu::Device& RenderContext::device() { return _binding->device(); }

wgpu::TextureView RenderContext::currentDrawableTexture() { return _swapchain.GetCurrentTextureView(); }

void RenderContext::resize(uint32_t width, uint32_t height) {
    if (width != _width || height != _height) {
        _swapchain.Configure(drawableTextureFormat(), wgpu::TextureUsage::RenderAttachment, width, height);
    }
    _width = width;
    _height = height;
}

wgpu::TextureFormat RenderContext::drawableTextureFormat() { return _binding->preferredSwapChainTextureFormat(); }

void RenderContext::present() { _swapchain.Present(); }

}  // namespace vox
