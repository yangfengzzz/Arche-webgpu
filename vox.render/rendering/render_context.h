//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/utils/backend_binding.h"

namespace vox {
class RenderContext {
public:
    RenderContext(BackendBinding* binding, uint32_t width, uint32_t height);

    wgpu::Device& device();

    void resize(uint32_t width, uint32_t height);

    wgpu::TextureView currentDrawableTexture();

    wgpu::TextureFormat drawableTextureFormat();

    void present();

private:
    BackendBinding* _binding{nullptr};
    uint32_t _width;
    uint32_t _height;

    wgpu::SwapChain _swapchain;
};

}  // namespace vox