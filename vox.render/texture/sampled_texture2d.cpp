//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texture2d.h"
#include <vector>

namespace vox {
SampledTexture2D::SampledTexture2D(wgpu::Device& device,
                                   uint32_t width,
                                   uint32_t height,
                                   wgpu::TextureFormat format,
                                   wgpu::TextureUsage usage,
                                   bool mipmap):
SampledTexture(device) {
    _textureDesc.size.width = width;
    _textureDesc.size.height = height;
    _textureDesc.format = format;
    _textureDesc.usage = usage;
    _textureDesc.mipLevelCount = _getMipmapCount(mipmap);
    _nativeTexture = device.CreateTexture(&_textureDesc);
    
    setMinFilterMode(wgpu::FilterMode::Linear);
    setMagFilterMode(wgpu::FilterMode::Linear);
    setAddressModeU(wgpu::AddressMode::Repeat);
    setAddressModeV(wgpu::AddressMode::Repeat);
}

void SampledTexture2D::setPixelBuffer(std::vector<uint8_t>& data,
                                      uint32_t width, uint32_t height, uint32_t offset,
                                      uint32_t mipLevel, uint32_t x, uint32_t y) {
    wgpu::BufferDescriptor descriptor;
    descriptor.size = data.size();
    descriptor.usage = wgpu::BufferUsage::CopySrc | wgpu::BufferUsage::CopyDst;
    wgpu::Buffer stagingBuffer = _device.CreateBuffer(&descriptor);
    _device.GetQueue().WriteBuffer(stagingBuffer, 0, data.data(), data.size());
    
    wgpu::ImageCopyBuffer imageCopyBuffer = _createImageCopyBuffer(stagingBuffer, offset, bytesPerPixel(_textureDesc.format) * width);
    wgpu::ImageCopyTexture imageCopyTexture = _createImageCopyTexture(mipLevel, {x, y, 0});
    wgpu::Extent3D copySize = {width, height, 1};
    
    wgpu::CommandEncoder encoder = _device.CreateCommandEncoder();
    encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);
    
    wgpu::CommandBuffer copy = encoder.Finish();
    _device.GetQueue().Submit(1, &copy);
}


}
