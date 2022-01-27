//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef render_context_hpp
#define render_context_hpp

#include "utils/backend_binding.h"

namespace vox {
class RenderContext {
public:
    RenderContext(BackendBinding* binding, uint32_t width, uint32_t height);
    
    wgpu::Device& device();
    
    void resize(uint32_t width, uint32_t height);

    wgpu::TextureView currentDrawableTexture();
    
    wgpu::TextureFormat drawableTextureFormat();

    wgpu::TextureView depthStencilTexture();
    
    wgpu::TextureFormat depthStencilTextureFormat();
    
    void present();
        
private:
    wgpu::TextureView _createDepthStencilView(uint32_t width, uint32_t height);
    
private:
    BackendBinding* _binding{nullptr};
    uint32_t _width;
    uint32_t _height;
    
    wgpu::SwapChain _swapchain;
    wgpu::TextureView _depthStencilTexture;
    wgpu::TextureFormat _depthStencilTextureFormat = wgpu::TextureFormat::Depth24PlusStencil8;
};

}

#endif /* render_context_hpp */
