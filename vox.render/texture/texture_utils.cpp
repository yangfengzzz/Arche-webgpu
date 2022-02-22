//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "texture_utils.h"
#include "sampled_texture2d.h"

namespace vox {
void TextureUtils::buildTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                     const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                     wgpu::Texture& textureArray,
                                     wgpu::CommandEncoder& commandEncoder) {
    
}

void TextureUtils::buildCubeTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                         const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                         wgpu::Texture& textureArray,
                                         wgpu::CommandEncoder& commandEncoder) {
    
}

void TextureUtils::buildAtlas(const std::array<wgpu::Texture, 4> &textures,
                              wgpu::Texture& atlas,
                              wgpu::CommandEncoder& commandEncoder) {
    
}

void TextureUtils::buildCubeAtlas(const std::array<wgpu::Texture, 6> &textures,
                                  wgpu::Texture& atlas,
                                  wgpu::CommandEncoder& commandEncoder) {
    
}

}
