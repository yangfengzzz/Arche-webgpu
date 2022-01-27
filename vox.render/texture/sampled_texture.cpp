//
//  sampler_texture.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#include "sampled_texture.h"
#include <math.h>

namespace vox {
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

//MARK: - Sampler
wgpu::AddressMode SampledTexture::addressModeU() {
    return _samplerDesc.addressModeU;
}

void SampledTexture::setAddressModeU(wgpu::AddressMode value) {
    _samplerDesc.addressModeU = value;
}

wgpu::AddressMode SampledTexture::addressModeV() {
    return _samplerDesc.addressModeV;
}

void SampledTexture::setAddressModeV(wgpu::AddressMode value) {
    _samplerDesc.addressModeV = value;
}

wgpu::FilterMode SampledTexture::minFilterMode() {
    return _samplerDesc.minFilter;
}

void SampledTexture::setMinFilterMode(wgpu::FilterMode value) {
    _samplerDesc.minFilter = value;
}

wgpu::FilterMode SampledTexture::magFilterMode() {
    return _samplerDesc.magFilter;
}

void SampledTexture::setMagFilterMode(wgpu::FilterMode value) {
    _samplerDesc.magFilter = value;
}

uint16_t SampledTexture::anisoLevel() {
    return _samplerDesc.maxAnisotropy;
}

void SampledTexture::setAnisoLevel(uint16_t value) {
    _samplerDesc.maxAnisotropy = value;
}

}
