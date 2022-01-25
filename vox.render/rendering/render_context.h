//
//  render_context.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/25.
//

#ifndef render_context_hpp
#define render_context_hpp

#include <dawn/webgpu_cpp.h>

namespace vox {
class RenderContext {
public:
    static constexpr wgpu::TextureFormat DEPTH_STENCIL_TEXTURE_FORMAT = wgpu::TextureFormat::Depth24PlusStencil8;
    static constexpr wgpu::TextureFormat DRAWBLE_TEXTURE_FORMAT = wgpu::TextureFormat::BGRA8Unorm;
    
    RenderContext(wgpu::Device& device, uint32_t width, uint32_t height);
    
    wgpu::Device &device();
    
    wgpu::TextureView currentDrawableTexture();

    wgpu::TextureView depthStencilTexture();
    
    void resize(uint32_t width, uint32_t height);
        
private:
    wgpu::TextureView _createDepthStencilView(uint32_t width, uint32_t height);
    
private:
    wgpu::Device& _device;
    uint32_t _width;
    uint32_t _height;
    bool _isDirty{false};
    
    wgpu::SwapChain _swapchain;
    wgpu::TextureView _depthStencilTexture;
};

}

#endif /* render_context_hpp */
