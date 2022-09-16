//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/texture/sampled_texturecube.h"

#include <array>

namespace vox {
SampledTextureCube::SampledTextureCube(wgpu::Device& device,
                                       uint32_t width,
                                       uint32_t height,
                                       uint32_t depthOrArrayLayers,
                                       wgpu::TextureFormat format,
                                       wgpu::TextureUsage usage,
                                       bool mipmap)
    : SampledTexture(device) {
    _textureDesc.size.width = width;
    _textureDesc.size.height = height;
    _textureDesc.size.depthOrArrayLayers = 6 * depthOrArrayLayers;
    _textureDesc.format = format;
    _textureDesc.usage = usage;
    _textureDesc.mipLevelCount = _getMipmapCount(mipmap);

    _dimension = wgpu::TextureViewDimension::Cube;
    _nativeTexture = device.CreateTexture(&_textureDesc);
}

wgpu::TextureView SampledTextureCube::textureView() {
    wgpu::TextureViewDescriptor desc;
    desc.format = _textureDesc.format;
    desc.dimension = _dimension;
    desc.mipLevelCount = _textureDesc.mipLevelCount;
    desc.arrayLayerCount = _textureDesc.size.depthOrArrayLayers;
    desc.aspect = wgpu::TextureAspect::All;
    return _nativeTexture.CreateView(&desc);
}

SampledTexture2DViewPtr SampledTextureCube::textureView2D(uint32_t mipmapLevel, uint32_t layer) {
    return std::make_shared<SampledTexture2DView>(
            _device, [ mipmapLevel, layer, this ]() -> auto{
                wgpu::TextureViewDescriptor desc;
                desc.format = _textureDesc.format;
                desc.dimension = wgpu::TextureViewDimension::e2D;
                desc.baseMipLevel = mipmapLevel;
                desc.mipLevelCount = 1;
                desc.baseArrayLayer = layer;
                desc.arrayLayerCount = 1;
                desc.aspect = wgpu::TextureAspect::All;
                return _nativeTexture.CreateView(&desc);
            });
}

void SampledTextureCube::setPixelBuffer(std::array<Image*, 6> images) {
    for (uint32_t i = 0; i < 6; i++) {
        const Image* image = images[i];
        const std::vector<uint8_t>& data = image->data();

        wgpu::BufferDescriptor descriptor;
        descriptor.size = data.size();
        descriptor.usage = wgpu::BufferUsage::CopySrc | wgpu::BufferUsage::CopyDst;
        wgpu::Buffer stagingBuffer = _device.CreateBuffer(&descriptor);
        _device.GetQueue().WriteBuffer(stagingBuffer, 0, data.data(), data.size());

        auto width = image->mipmaps().at(0).extent.width;
        auto height = image->mipmaps().at(0).extent.height;
        auto offset = image->mipmaps().at(0).offset;
        wgpu::ImageCopyBuffer imageCopyBuffer =
                _createImageCopyBuffer(stagingBuffer, offset, bytesPerPixel(_textureDesc.format) * width);
        wgpu::ImageCopyTexture imageCopyTexture = _createImageCopyTexture(0, {0, 0, i});
        wgpu::Extent3D copySize = {width, height, 1};

        wgpu::CommandEncoder encoder = _device.CreateCommandEncoder();
        encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);

        wgpu::CommandBuffer copy = encoder.Finish();
        _device.GetQueue().Submit(1, &copy);
    }
}

}  // namespace vox
