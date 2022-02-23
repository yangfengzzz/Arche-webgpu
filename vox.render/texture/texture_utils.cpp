//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "texture_utils.h"
#include "sampled_texture2d.h"
#include <array>

namespace vox {
void TextureUtils::buildTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                     const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                     uint32_t width, uint32_t height,
                                     wgpu::Texture& textureArray,
                                     wgpu::CommandEncoder& commandEncoder) {
    wgpu::ImageCopyTexture destination;
    destination.texture = textureArray;
    
    wgpu::ImageCopyTexture source;
    
    wgpu::Extent3D copySize;
    copySize.width = width;
    copySize.height = height;
    copySize.depthOrArrayLayers = 1;
    
    for (auto iter = texturesBegin; iter < texturesEnd; iter++) {
        destination.origin.z = static_cast<uint32_t>(iter - texturesBegin);
        source.texture = *iter;
        
        commandEncoder.CopyTextureToTexture(&source, &destination, &copySize);
    }
}

void TextureUtils::buildCubeTextureArray(const std::vector<wgpu::Texture>::iterator &texturesBegin,
                                         const std::vector<wgpu::Texture>::iterator &texturesEnd,
                                         uint32_t width, uint32_t height,
                                         wgpu::Texture& textureArray,
                                         wgpu::CommandEncoder& commandEncoder) {
    wgpu::ImageCopyTexture destination;
    destination.texture = textureArray;
    
    wgpu::ImageCopyTexture source;
    
    wgpu::Extent3D copySize;
    copySize.width = width;
    copySize.height = height;
    copySize.depthOrArrayLayers = 6;
    
    for (auto iter = texturesBegin; iter < texturesEnd; iter++) {
        destination.origin.z = 6 * static_cast<uint32_t>(iter - texturesBegin);
        source.texture = *iter;
        
        commandEncoder.CopyTextureToTexture(&source, &destination, &copySize);
    }
}

}
