//
//  sampler_texture.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#ifndef sampled_texture_hpp
#define sampled_texture_hpp

#include <dawn/webgpu_cpp.h>

namespace vox {
class SampledTexture {
public:
    /**
     * The width of the texture.
     */
    float width();
    
    /**
     * The height of the texture.
     */
    float height();
    
    /**
     * Texture mipmapping count.
     */
    uint32_t mipmapCount();
    
    /**
     * Texture format.
     */
    wgpu::TextureFormat format();
    
public:
    /**
     * Wrapping mode for texture coordinate S.
     */
    wgpu::AddressMode addressModeU();
    
    void setAddressModeU(wgpu::AddressMode value);
    
    /**
     * Wrapping mode for texture coordinate T.
     */
    wgpu::AddressMode addressModeV();
    
    void setAddressModeV(wgpu::AddressMode value);
    
    /**
     * Filter mode for texture.
     */
    wgpu::FilterMode minFilterMode();
    
    void setMinFilterMode(wgpu::FilterMode value);
    
    /**
     * Filter mode for texture.
     */
    wgpu::FilterMode magFilterMode();
    
    void setMagFilterMode(wgpu::FilterMode value);
    
    /**
     * Anisotropic level for texture.
     */
    uint16_t anisoLevel();
    
    void setAnisoLevel(uint16_t value);
    
protected:
    uint32_t _getMipmapCount(bool mipmap);
    
    wgpu::Texture _nativeTexture;
    wgpu::Sampler _nativeSampler;
    wgpu::TextureDescriptor _textureDesc;
    wgpu::SamplerDescriptor _samplerDesc;
};

}

#endif /* sampler_texture_hpp */
