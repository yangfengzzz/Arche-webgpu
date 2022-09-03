//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/texture/sampled_texture3d.h"

namespace vox {
SampledTexture3D::SampledTexture3D(wgpu::Device& device,
                                   uint32_t width,
                                   uint32_t height,
                                   uint32_t depthOrArrayLayers,
                                   wgpu::TextureFormat format,
                                   wgpu::TextureUsage usage,
                                   bool mipmap)
    : SampledTexture(device) {
    _textureDesc.size.width = width;
    _textureDesc.size.height = height;
    _textureDesc.size.depthOrArrayLayers = depthOrArrayLayers;
    _textureDesc.format = format;
    _textureDesc.usage = usage;
    _textureDesc.mipLevelCount = _getMipmapCount(mipmap);

    _dimension = wgpu::TextureViewDimension::e3D;
    _nativeTexture = device.CreateTexture(&_textureDesc);
}

wgpu::TextureView SampledTexture3D::textureView() {
    wgpu::TextureViewDescriptor desc;
    desc.format = _textureDesc.format;
    desc.dimension = _dimension;
    desc.mipLevelCount = _textureDesc.mipLevelCount;
    desc.arrayLayerCount = _textureDesc.size.depthOrArrayLayers;
    desc.aspect = wgpu::TextureAspect::All;
    return _nativeTexture.CreateView(&desc);
}

}  // namespace vox
