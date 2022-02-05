//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sampled_texturecube_hpp
#define sampled_texturecube_hpp

#include "sampled_texture.h"
#include "image/image.h"

namespace vox {
class SampledTextureCube : public SampledTexture {
public:
    /**
     * Create TextureCube.
     * @param device - Define the engine to use to render this texture
     * @param width - Texture width
     * @param height - Texture height
     * @param format - Texture format. default  `TextureFormat.R8G8B8A8`
     * @param mipmap - Whether to use multi-level texture
     */
    SampledTextureCube(wgpu::Device& device,
                       uint32_t width,
                       uint32_t height,
                       wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Sint,
                       wgpu::TextureUsage usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
                       bool mipmap = true);
    
    wgpu::TextureView textureView() override;
    
    void setPixelBuffer(std::vector<Image*> images);
};
using SampledTextureCubePtr = std::shared_ptr<SampledTextureCube>;

}
#endif /* sampled_texturecube_hpp */
