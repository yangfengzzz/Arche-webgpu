//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.render/texture/sampled_texture.h"

namespace vox {
class TextureUtils {
public:
    // n -> 1
    static void buildTextureArray(const std::vector<wgpu::Texture>::iterator& texturesBegin,
                                  const std::vector<wgpu::Texture>::iterator& texturesEnd,
                                  uint32_t width,
                                  uint32_t height,
                                  wgpu::Texture& textureArray,
                                  wgpu::CommandEncoder& commandEncoder);

    // 6n -> 1
    static void buildCubeTextureArray(const std::vector<wgpu::Texture>::iterator& texturesBegin,
                                      const std::vector<wgpu::Texture>::iterator& texturesEnd,
                                      uint32_t width,
                                      uint32_t height,
                                      wgpu::Texture& textureArray,
                                      wgpu::CommandEncoder& commandEncoder);

    static wgpu::TextureSampleType sampleType(wgpu::TextureFormat format);
};

}  // namespace vox