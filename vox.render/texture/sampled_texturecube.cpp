//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texturecube.h"

namespace vox {
SampledTextureCube::SampledTextureCube(wgpu::Device& device,
                                       uint32_t width,
                                       uint32_t height,
                                       wgpu::TextureFormat format,
                                       wgpu::TextureUsage usage,
                                       bool mipmap):
SampledTexture(device) {
    _textureDesc.size.width = width;
    _textureDesc.size.height = height;
    _textureDesc.size.depthOrArrayLayers = 6;
    _textureDesc.format = format;
    _textureDesc.usage = usage;
    _textureDesc.mipLevelCount = _getMipmapCount(mipmap);
    _nativeTexture = device.CreateTexture(&_textureDesc);
    
    setMinFilterMode(wgpu::FilterMode::Linear);
    setMagFilterMode(wgpu::FilterMode::Linear);
    setAddressModeU(wgpu::AddressMode::Repeat);
    setAddressModeV(wgpu::AddressMode::Repeat);
}

wgpu::TextureView SampledTextureCube::textureView() {
    wgpu::TextureViewDescriptor desc;
    desc.format = _textureDesc.format;
    desc.dimension = wgpu::TextureViewDimension::Cube;
    desc.mipLevelCount = _textureDesc.mipLevelCount;
    desc.arrayLayerCount = _textureDesc.size.depthOrArrayLayers;
    desc.aspect = wgpu::TextureAspect::All;
    return _nativeTexture.CreateView(&desc);
}

void SampledTextureCube::setPixelBuffer(std::vector<Image*> images) {
    
}

}
