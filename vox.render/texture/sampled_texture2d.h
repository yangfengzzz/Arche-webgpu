//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sampled_texture2D_hpp
#define sampled_texture2D_hpp

#include "sampled_texture.h"
#include <memory>

namespace vox {
class SampledTexture2D : public SampledTexture {
public:
    /**
     * Create Texture2D.
     * @param device - Define the engine to use to render this texture
     * @param width - Texture width
     * @param height - Texture height
     * @param format - Texture format. default  `TextureFormat.R8G8B8A8`
     * @param mipmap - Whether to use multi-level texture
     */
    SampledTexture2D(wgpu::Device& device,
                     uint32_t width,
                     uint32_t height,
                     wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Sint,
                     bool mipmap = true);
    
    wgpu::Sampler& sampler();
    
private:
    wgpu::Device& _device;
};
using SampledTexture2DPtr = std::shared_ptr<SampledTexture2D>;

}

#endif /* sampled_texture2D_hpp */
