//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texture_utils.h"

namespace vox {
SampledTexturePtr SampledTextureUtils::createTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                                          const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                                          wgpu::CommandEncoder& commandEncoder) {
    return nullptr;
}

void SampledTextureUtils::createTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                             const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                             SampledTexturePtr textureArray,
                                             wgpu::CommandEncoder& commandEncoder) {
    
}

SampledTexturePtr SampledTextureUtils::createCubeTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                                              const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                                              wgpu::CommandEncoder& commandEncoder)  {
    return nullptr;
}

void SampledTextureUtils::createCubeTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                                 const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                                 SampledTexturePtr textureArray,
                                                 wgpu::CommandEncoder& commandEncoder) {
    
}

SampledTexturePtr SampledTextureUtils::createAtlas(const std::array<wgpu::Texture, 4> &textures,
                                                   wgpu::CommandEncoder& commandEncoder) {
    return nullptr;
}

void SampledTextureUtils::createAtlas(const std::array<wgpu::Texture, 4> &textures,
                                      wgpu::Texture& atlas,
                                      wgpu::CommandEncoder& commandEncoder) {
    
}

SampledTexturePtr SampledTextureUtils::createCubeAtlas(const std::array<wgpu::Texture, 6> &textures,
                                                       wgpu::CommandEncoder& commandEncoder) {
    return nullptr;
}

void SampledTextureUtils::createCubeAtlas(const std::array<wgpu::Texture, 6> &textures,
                                          wgpu::Texture& atlas,
                                          wgpu::CommandEncoder& commandEncoder){
    
}

}
