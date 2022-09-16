//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/texture/image.h"
#include "vox.render/texture/sampled_texture.h"

namespace vox {
class SampledTexture2D : public SampledTexture {
public:
    /**
     * Create Texture2D.
     * @param device - Define the engine to use to render this textureold
     * @param width - Texture width
     * @param height - Texture height
     * @param format - Texture format. default  `TextureFormat.R8G8B8A8`
     * @param mipmap - Whether to use multi-level textureold
     */
    SampledTexture2D(wgpu::Device& device,
                     uint32_t width,
                     uint32_t height,
                     uint32_t depthOrArrayLayers = 1,
                     wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm,
                     wgpu::TextureUsage usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
                     bool mipmap = true);

    wgpu::TextureView textureView() override;

    void setPixelBuffer(const std::vector<uint8_t>& data,
                        uint32_t width,
                        uint32_t height,
                        uint32_t mipLevel = 0,
                        uint32_t offset = 0,
                        uint32_t x = 0,
                        uint32_t y = 0);

    void setImageSource(const ::vox::Image* data);

protected:
    explicit SampledTexture2D(wgpu::Device& device);
};
using SampledTexture2DPtr = std::shared_ptr<SampledTexture2D>;

}  // namespace vox