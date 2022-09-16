//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <webgpu/webgpu_cpp.h>

#include <memory>

namespace vox {
uint32_t bytesPerPixel(wgpu::TextureFormat format);

class SampledTexture {
public:
    explicit SampledTexture(wgpu::Device& device);

    /**
     * The width of the textureold.
     */
    [[nodiscard]] uint32_t width() const;

    /**
     * The height of the textureold.
     */
    [[nodiscard]] uint32_t height() const;

    /**
     * The depthOrArrayLayers of the textureold.
     */
    [[nodiscard]] uint32_t depthOrArrayLayers() const;

    /**
     * Texture mipmapping count.
     */
    [[nodiscard]] uint32_t mipmapCount() const;

    /**
     * Texture format.
     */
    [[nodiscard]] wgpu::TextureFormat format() const;

    [[nodiscard]] uint32_t sampleCount() const;

    wgpu::Texture& texture();

public:
    wgpu::TextureViewDimension textureViewDimension();

    void setTextureViewDimension(wgpu::TextureViewDimension dim);

    virtual wgpu::TextureView textureView() = 0;

public:
    /**
     * Wrapping mode for textureold coordinate S.
     */
    [[nodiscard]] wgpu::AddressMode addressModeU() const;

    void setAddressModeU(wgpu::AddressMode value);

    /**
     * Wrapping mode for textureold coordinate T.
     */
    [[nodiscard]] wgpu::AddressMode addressModeV() const;

    void setAddressModeV(wgpu::AddressMode value);

    /**
     * Filter mode for textureold.
     */
    [[nodiscard]] wgpu::FilterMode minFilterMode() const;

    void setMinFilterMode(wgpu::FilterMode value);

    /**
     * Filter mode for textureold.
     */
    [[nodiscard]] wgpu::FilterMode magFilterMode() const;

    void setMagFilterMode(wgpu::FilterMode value);

    /**
     * Filter mode for textureold.
     */
    [[nodiscard]] wgpu::FilterMode mipmapFilter() const;

    void setMipmapFilter(wgpu::FilterMode value);

    /**
     * Anisotropic level for textureold.
     */
    [[nodiscard]] uint16_t anisoLevel() const;

    void setAnisoLevel(uint16_t value);

    [[nodiscard]] wgpu::CompareFunction compareFunction() const;

    void setCompareFunction(wgpu::CompareFunction function);

    wgpu::Sampler& sampler();

protected:
    [[nodiscard]] uint32_t _getMipmapCount(bool mipmap) const;

    wgpu::ImageCopyBuffer _createImageCopyBuffer(wgpu::Buffer buffer,
                                                 uint64_t offset,
                                                 uint32_t bytesPerRow,
                                                 uint32_t rowsPerImage = wgpu::kCopyStrideUndefined);

    wgpu::ImageCopyTexture _createImageCopyTexture(uint32_t level,
                                                   wgpu::Origin3D origin,
                                                   wgpu::TextureAspect aspect = wgpu::TextureAspect::All);

    wgpu::TextureDataLayout _createTextureDataLayout(uint64_t offset,
                                                     uint32_t bytesPerRow,
                                                     uint32_t rowsPerImage = wgpu::kCopyStrideUndefined);

    wgpu::Device& _device;
    wgpu::Texture _nativeTexture;
    wgpu::Sampler _nativeSampler;
    wgpu::TextureDescriptor _textureDesc;
    wgpu::SamplerDescriptor _samplerDesc;
    wgpu::TextureViewDimension _dimension = wgpu::TextureViewDimension::Undefined;
    bool _isDirty{false};
};
using SampledTexturePtr = std::shared_ptr<SampledTexture>;

}  // namespace vox