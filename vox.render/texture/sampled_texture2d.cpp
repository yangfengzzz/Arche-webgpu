//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texture2d.h"

namespace vox {
SampledTexture2D::SampledTexture2D(wgpu::Device& device,
                                   uint32_t width,
                                   uint32_t height,
                                   wgpu::TextureFormat format,
                                   bool mipmap):
_device(device) {
    _textureDesc.size.width = width;
    _textureDesc.size.height = height;
    _textureDesc.format = format;
    _textureDesc.mipLevelCount = _getMipmapCount(mipmap);
    _nativeTexture = device.CreateTexture(&_textureDesc);
    
    setMinFilterMode(wgpu::FilterMode::Linear);
    setMagFilterMode(wgpu::FilterMode::Linear);
    setAddressModeU(wgpu::AddressMode::Repeat);
    setAddressModeV(wgpu::AddressMode::Repeat);
}

wgpu::Sampler& SampledTexture2D::sampler() {
    if (_isDirty) {
        _nativeSampler = _device.CreateSampler(&_samplerDesc);
        _isDirty = false;
    }
    return _nativeSampler;
}

}
