//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/texture/sampled_texture.h"

namespace vox {
class SampledTexture3D : public SampledTexture {
public:
    /**
     * Create TextureCube.
     * @param device - Define the engine to use to render this textureold
     * @param width - Texture width
     * @param height - Texture height
     * @param format - Texture format. default  `TextureFormat.R8G8B8A8`
     * @param mipmap - Whether to use multi-level textureold
     */
    SampledTexture3D(wgpu::Device& device,
                     uint32_t width,
                     uint32_t height,
                     uint32_t depthOrArrayLayers = 1,
                     wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm,
                     wgpu::TextureUsage usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
                     bool mipmap = true);

    wgpu::TextureView textureView() override;
};
using SampledTexture3DPtr = std::shared_ptr<SampledTexture3D>;

}  // namespace vox