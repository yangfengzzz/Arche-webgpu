//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sampled_texture_utils_hpp
#define sampled_texture_utils_hpp

#include "sampled_texture.h"
#include <vector>

namespace vox {
class TextureUtils {
public:
    // n -> 1
    static void buildTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                  const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                  uint32_t width, uint32_t height,
                                  wgpu::Texture& textureArray,
                                  wgpu::CommandEncoder& commandEncoder);
    
    // 6n -> 1
    static void buildCubeTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                      const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                      uint32_t width, uint32_t height,
                                      wgpu::Texture& textureArray,
                                      wgpu::CommandEncoder& commandEncoder);
    
    // 4 -> 1
    static void buildAtlas(const std::array<wgpu::Texture, 4> &textures,
                           uint32_t width, uint32_t height,
                           wgpu::Texture& atlas,
                           wgpu::CommandEncoder& commandEncoder);
    
    // 6 -> 1
    static void buildCubeAtlas(const std::array<wgpu::Texture, 6> &textures,
                               uint32_t width, uint32_t height,
                               wgpu::Texture& atlas,
                               wgpu::CommandEncoder& commandEncoder);
};

}

#endif /* sampled_texture_utils_hpp */
