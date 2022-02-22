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
class SampledTextureUtils {
public:
    static SampledTexturePtr createTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                                const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                                wgpu::CommandEncoder& commandEncoder);
    
    static SampledTexturePtr createCubeTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                                    const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                                    wgpu::CommandEncoder& commandEncoder);
    
    static SampledTexturePtr createAtlas(const std::array<wgpu::Texture, 4> &textures,
                                         wgpu::CommandEncoder& commandEncoder);
    
    static SampledTexturePtr createCubeAtlas(const std::array<wgpu::Texture, 6> &textures,
                                             wgpu::CommandEncoder& commandEncoder);
};

}

#endif /* sampled_texture_utils_hpp */
