//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once
#include <webgpu/webgpu_cpp.h>

namespace vox {
class Sampler {
public:
    explicit Sampler(wgpu::Device& device,
                     wgpu::AddressMode addressModeU = wgpu::AddressMode::ClampToEdge,
                     wgpu::AddressMode addressModeV = wgpu::AddressMode::ClampToEdge,
                     wgpu::AddressMode addressModeW = wgpu::AddressMode::ClampToEdge,
                     wgpu::FilterMode minFilterMode = wgpu::FilterMode::Nearest,
                     wgpu::FilterMode magFilterMode = wgpu::FilterMode::Nearest,
                     wgpu::FilterMode mipmapFilter = wgpu::FilterMode::Nearest,
                     uint16_t maxAnisotropy = 1,
                     float lodMinClamp = 0.0f,
                     float lodMaxClamp = 1000.0f,
                     wgpu::CompareFunction compare = wgpu::CompareFunction::Undefined);

    /**
     * Wrapping mode for sampler coordinate U.
     */
    [[nodiscard]] wgpu::AddressMode addressModeU() const;

    /**
     * Wrapping mode for sampler coordinate V.
     */
    [[nodiscard]] wgpu::AddressMode addressModeV() const;

    /**
     * Wrapping mode for sampler coordinate W.
     */
    [[nodiscard]] wgpu::AddressMode addressModeW() const;

    /**
     * Filter mode for sampler.
     */
    [[nodiscard]] wgpu::FilterMode minFilterMode() const;

    /**
     * Filter mode for sampler.
     */
    [[nodiscard]] wgpu::FilterMode magFilterMode() const;

    /**
     * Filter mode for sampler.
     */
    [[nodiscard]] wgpu::FilterMode mipmapFilter() const;

    /**
     * Anisotropic level for sampler.
     */
    [[nodiscard]] uint16_t maxAnisotropy() const;

    [[nodiscard]] float lodMinClamp() const;

    [[nodiscard]] float lodMaxClamp() const;

    [[nodiscard]] wgpu::CompareFunction compare() const;

    wgpu::Sampler& sampler();

private:
    wgpu::SamplerDescriptor _desc;
    wgpu::Sampler _handle;
};
}  // namespace vox