//
//  sampled_texture2D.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#include "sampled_texture2D.h"

namespace vox {
SampledTexture2D::SampledTexture2D(wgpu::Device& device,
                                   uint32_t width,
                                   uint32_t height,
                                   wgpu::TextureFormat format,
                                   bool mipmap) {
    _textureDesc.size.width = width;
    _textureDesc.size.height = height;
    _textureDesc.format = format;
    _textureDesc.mipLevelCount = _getMipmapCount(mipmap);
    
    setMinFilterMode(wgpu::FilterMode::Linear);
    setMagFilterMode(wgpu::FilterMode::Linear);
    setAddressModeU(wgpu::AddressMode::Repeat);
    setAddressModeV(wgpu::AddressMode::Repeat);
}

}
