//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texturecube.h"
#include <array>

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

SampledTexture2DViewPtr SampledTextureCube::createView(uint32_t layer) {
    wgpu::TextureViewDescriptor desc;
    desc.format = _textureDesc.format;
    desc.dimension = wgpu::TextureViewDimension::Cube;
    desc.mipLevelCount = _textureDesc.mipLevelCount;
    desc.baseArrayLayer = layer;
    desc.arrayLayerCount = 1;
    desc.aspect = wgpu::TextureAspect::All;
    return std::make_shared<SampledTexture2DView>(_device, _nativeTexture.CreateView(&desc));
}

void SampledTextureCube::setPixelBuffer(std::array<Image*, 6> images) {
    for (uint32_t i = 0; i < 6; i++) {
        const Image* image = images[i];
        const std::vector<uint8_t> &data = image->data();
        
        wgpu::BufferDescriptor descriptor;
        descriptor.size = data.size();
        descriptor.usage = wgpu::BufferUsage::CopySrc | wgpu::BufferUsage::CopyDst;
        wgpu::Buffer stagingBuffer = _device.CreateBuffer(&descriptor);
        _device.GetQueue().WriteBuffer(stagingBuffer, 0, data.data(), data.size());
        
        auto width = image->mipmaps().at(0).extent.width;
        auto height = image->mipmaps().at(0).extent.height;
        auto offset = image->mipmaps().at(0).offset;
        wgpu::ImageCopyBuffer imageCopyBuffer = _createImageCopyBuffer(stagingBuffer, offset, bytesPerPixel(_textureDesc.format) * width);
        wgpu::ImageCopyTexture imageCopyTexture = _createImageCopyTexture(0, {0, 0, i});
        wgpu::Extent3D copySize = {width, height, 1};
        
        wgpu::CommandEncoder encoder = _device.CreateCommandEncoder();
        encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);
        
        wgpu::CommandBuffer copy = encoder.Finish();
        _device.GetQueue().Submit(1, &copy);
    }
}

}
