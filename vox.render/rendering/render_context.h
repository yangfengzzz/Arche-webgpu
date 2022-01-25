//
//  render_context.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/25.
//

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
    bool _isDirty{false};
    
    wgpu::SwapChain _swapchain;
    wgpu::TextureView _depthStencilTexture;
    wgpu::TextureFormat _depthStencilTextureFormat = wgpu::TextureFormat::Depth24PlusStencil8;
};

}

#endif /* render_context_hpp */
