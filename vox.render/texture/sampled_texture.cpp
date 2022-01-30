//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texture.h"
#include <math.h>

namespace vox {
SampledTexture::SampledTexture(wgpu::Device& device):
_device(device) {
}

float SampledTexture::width() {
    return _textureDesc.size.width;
}

float SampledTexture::height() {
    return _textureDesc.size.height;
}

uint32_t SampledTexture::mipmapCount() {
    return _textureDesc.mipLevelCount;
}

wgpu::TextureFormat SampledTexture::format() {
    return _textureDesc.format;
}

uint32_t SampledTexture::_getMipmapCount(bool mipmap) {
    return mipmap ? log2(fmax(_textureDesc.size.width, _textureDesc.size.height)) + 1 : 1;
}

wgpu::Texture& SampledTexture::texture() {
    return _nativeTexture;
}

wgpu::Sampler& SampledTexture::sampler() {
    if (_isDirty) {
        _nativeSampler = _device.CreateSampler(&_samplerDesc);
        _isDirty = false;
    }
    return _nativeSampler;
}

//MARK: - Sampler
wgpu::AddressMode SampledTexture::addressModeU() {
    return _samplerDesc.addressModeU;
}

void SampledTexture::setAddressModeU(wgpu::AddressMode value) {
    _samplerDesc.addressModeU = value;
    _isDirty = true;
}

wgpu::AddressMode SampledTexture::addressModeV() {
    return _samplerDesc.addressModeV;
}

void SampledTexture::setAddressModeV(wgpu::AddressMode value) {
    _samplerDesc.addressModeV = value;
    _isDirty = true;
}

wgpu::FilterMode SampledTexture::minFilterMode() {
    return _samplerDesc.minFilter;
}

void SampledTexture::setMinFilterMode(wgpu::FilterMode value) {
    _samplerDesc.minFilter = value;
    _isDirty = true;
}

wgpu::FilterMode SampledTexture::magFilterMode() {
    return _samplerDesc.magFilter;
}

void SampledTexture::setMagFilterMode(wgpu::FilterMode value) {
    _samplerDesc.magFilter = value;
    _isDirty = true;
}

uint16_t SampledTexture::anisoLevel() {
    return _samplerDesc.maxAnisotropy;
}

void SampledTexture::setAnisoLevel(uint16_t value) {
    _samplerDesc.maxAnisotropy = value;
    _isDirty = true;
}

//MARK: - Helper
wgpu::ImageCopyBuffer SampledTexture::_createImageCopyBuffer(wgpu::Buffer buffer,
                                                             uint64_t offset,
                                                             uint32_t bytesPerRow,
                                                             uint32_t rowsPerImage) {
    wgpu::ImageCopyBuffer imageCopyBuffer = {};
    imageCopyBuffer.buffer = buffer;
    imageCopyBuffer.layout = _createTextureDataLayout(offset, bytesPerRow, rowsPerImage);
    
    return imageCopyBuffer;
}

wgpu::ImageCopyTexture SampledTexture::_createImageCopyTexture(uint32_t mipLevel,
                                                               wgpu::Origin3D origin,
                                                               wgpu::TextureAspect aspect) {
    wgpu::ImageCopyTexture imageCopyTexture;
    imageCopyTexture.texture = _nativeTexture;
    imageCopyTexture.mipLevel = mipLevel;
    imageCopyTexture.origin = origin;
    imageCopyTexture.aspect = aspect;
    
    return imageCopyTexture;
}

wgpu::TextureDataLayout SampledTexture::_createTextureDataLayout(uint64_t offset,
                                                                 uint32_t bytesPerRow,
                                                                 uint32_t rowsPerImage) {
    wgpu::TextureDataLayout textureDataLayout;
    textureDataLayout.offset = offset;
    textureDataLayout.bytesPerRow = bytesPerRow;
    textureDataLayout.rowsPerImage = rowsPerImage;
    
    return textureDataLayout;
}

}
