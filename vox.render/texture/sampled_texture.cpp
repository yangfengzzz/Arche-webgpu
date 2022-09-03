//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/texture/sampled_texture.h"

#include <cmath>

namespace vox {
SampledTexture::SampledTexture(wgpu::Device& device) : _device(device) {
    setMinFilterMode(wgpu::FilterMode::Linear);
    setMagFilterMode(wgpu::FilterMode::Linear);
    setMipmapFilter(wgpu::FilterMode::Linear);
    setAddressModeU(wgpu::AddressMode::Repeat);
    setAddressModeV(wgpu::AddressMode::Repeat);
}

uint32_t SampledTexture::width() const { return _textureDesc.size.width; }

uint32_t SampledTexture::height() const { return _textureDesc.size.height; }

uint32_t SampledTexture::depthOrArrayLayers() const { return _textureDesc.size.depthOrArrayLayers; }

uint32_t SampledTexture::mipmapCount() const { return _textureDesc.mipLevelCount; }

wgpu::TextureFormat SampledTexture::format() const { return _textureDesc.format; }

wgpu::TextureViewDimension SampledTexture::textureViewDimension() { return _dimension; }

void SampledTexture::setTextureViewDimension(wgpu::TextureViewDimension dim) { _dimension = dim; }

uint32_t SampledTexture::_getMipmapCount(bool mipmap) const {
    return mipmap ? log2(fmax(_textureDesc.size.width, _textureDesc.size.height)) + 1 : 1;
}

wgpu::Texture& SampledTexture::texture() { return _nativeTexture; }

wgpu::Sampler& SampledTexture::sampler() {
    if (_isDirty) {
        _nativeSampler = _device.CreateSampler(&_samplerDesc);
        _isDirty = false;
    }
    return _nativeSampler;
}

// MARK: - Sampler
wgpu::AddressMode SampledTexture::addressModeU() const { return _samplerDesc.addressModeU; }

void SampledTexture::setAddressModeU(wgpu::AddressMode value) {
    _samplerDesc.addressModeU = value;
    _isDirty = true;
}

wgpu::AddressMode SampledTexture::addressModeV() const { return _samplerDesc.addressModeV; }

void SampledTexture::setAddressModeV(wgpu::AddressMode value) {
    _samplerDesc.addressModeV = value;
    _isDirty = true;
}

wgpu::FilterMode SampledTexture::minFilterMode() const { return _samplerDesc.minFilter; }

void SampledTexture::setMinFilterMode(wgpu::FilterMode value) {
    _samplerDesc.minFilter = value;
    _isDirty = true;
}

wgpu::FilterMode SampledTexture::magFilterMode() const { return _samplerDesc.magFilter; }

void SampledTexture::setMagFilterMode(wgpu::FilterMode value) {
    _samplerDesc.magFilter = value;
    _isDirty = true;
}

wgpu::FilterMode SampledTexture::mipmapFilter() const { return _samplerDesc.mipmapFilter; }

void SampledTexture::setMipmapFilter(wgpu::FilterMode value) {
    _samplerDesc.mipmapFilter = value;
    _isDirty = true;
}

uint16_t SampledTexture::anisoLevel() const { return _samplerDesc.maxAnisotropy; }

void SampledTexture::setAnisoLevel(uint16_t value) {
    _samplerDesc.maxAnisotropy = value;
    _isDirty = true;
}

wgpu::CompareFunction SampledTexture::compareFunction() const { return _samplerDesc.compare; }

void SampledTexture::setCompareFunction(wgpu::CompareFunction function) {
    _samplerDesc.compare = function;
    _isDirty = true;
}

// MARK: - Helper
wgpu::ImageCopyBuffer SampledTexture::_createImageCopyBuffer(wgpu::Buffer buffer,
                                                             uint64_t offset,
                                                             uint32_t bytesPerRow,
                                                             uint32_t rowsPerImage) {
    wgpu::ImageCopyBuffer imageCopyBuffer = {};
    imageCopyBuffer.buffer = std::move(buffer);
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

uint32_t bytesPerPixel(wgpu::TextureFormat format) {
    switch (format) {
        case wgpu::TextureFormat::R8Unorm:
        case wgpu::TextureFormat::R8Snorm:
        case wgpu::TextureFormat::R8Uint:
        case wgpu::TextureFormat::R8Sint:
            return 1;

        case wgpu::TextureFormat::R16Uint:
        case wgpu::TextureFormat::R16Sint:
        case wgpu::TextureFormat::R16Float:
        case wgpu::TextureFormat::RG8Unorm:
        case wgpu::TextureFormat::RG8Snorm:
            return 2;

        case wgpu::TextureFormat::R32Float:
        case wgpu::TextureFormat::R32Uint:
        case wgpu::TextureFormat::R32Sint:
        case wgpu::TextureFormat::RG16Uint:
        case wgpu::TextureFormat::RG16Sint:
        case wgpu::TextureFormat::RG16Float:
        case wgpu::TextureFormat::RGBA8Unorm:
        case wgpu::TextureFormat::RGBA8UnormSrgb:
        case wgpu::TextureFormat::RGBA8Snorm:
        case wgpu::TextureFormat::RGBA8Uint:
        case wgpu::TextureFormat::RGBA8Sint:
        case wgpu::TextureFormat::BGRA8Unorm:
        case wgpu::TextureFormat::BGRA8UnormSrgb:
            return 4;

        case wgpu::TextureFormat::RG32Float:
        case wgpu::TextureFormat::RG32Uint:
        case wgpu::TextureFormat::RG32Sint:
        case wgpu::TextureFormat::RGBA16Uint:
        case wgpu::TextureFormat::RGBA16Sint:
            return 8;

        case wgpu::TextureFormat::RGBA32Float:
        case wgpu::TextureFormat::RGBA32Sint:
            return 16;

        default:
            assert(false && "undefined");
            throw std::exception();
    }
}

}  // namespace vox
