//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/sampler.h"

namespace vox {
Sampler::Sampler(wgpu::Device& device,
                 wgpu::AddressMode addressModeU,
                 wgpu::AddressMode addressModeV,
                 wgpu::AddressMode addressModeW,
                 wgpu::FilterMode minFilterMode,
                 wgpu::FilterMode magFilterMode,
                 wgpu::FilterMode mipmapFilter,
                 uint16_t maxAnisotropy,
                 float lodMinClamp,
                 float lodMaxClamp,
                 wgpu::CompareFunction compare) {
    _desc = wgpu::SamplerDescriptor();
    _desc.addressModeV = addressModeV;
    _desc.addressModeU = addressModeU;
    _desc.addressModeW = addressModeW;
    _desc.minFilter = minFilterMode;
    _desc.magFilter = magFilterMode;
    _desc.mipmapFilter = mipmapFilter;
    _desc.maxAnisotropy = maxAnisotropy;
    _desc.lodMinClamp = lodMinClamp;
    _desc.lodMaxClamp = lodMaxClamp;
    _desc.compare = compare;
}

wgpu::AddressMode Sampler::addressModeU() const { return _desc.addressModeU; }

wgpu::AddressMode Sampler::addressModeV() const { return _desc.addressModeV; }

wgpu::AddressMode Sampler::addressModeW() const { return _desc.addressModeW; }

wgpu::FilterMode Sampler::minFilterMode() const { return _desc.minFilter; }

wgpu::FilterMode Sampler::magFilterMode() const { return _desc.magFilter; }

wgpu::FilterMode Sampler::mipmapFilter() const { return _desc.mipmapFilter; }

uint16_t Sampler::maxAnisotropy() const { return _desc.maxAnisotropy; }

float Sampler::lodMinClamp() const { return _desc.lodMinClamp; }

float Sampler::lodMaxClamp() const { return _desc.lodMaxClamp; }

wgpu::CompareFunction Sampler::compare() const { return _desc.compare; }

wgpu::Sampler& Sampler::sampler() { return _handle; }
}  // namespace vox