//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sampled_texture_hpp
#define sampled_texture_hpp

#include <webgpu/webgpu_cpp.h>
#include <memory>

namespace vox {
uint32_t bytesPerPixel(wgpu::TextureFormat format);

class SampledTexture {
public:
    SampledTexture(wgpu::Device& device);
    
    /**
     * The width of the texture.
     */
    uint32_t width();
    
    /**
     * The height of the texture.
     */
    uint32_t height();
    
    /**
     * The depthOrArrayLayers of the texture.
     */
    uint32_t depthOrArrayLayers();
    
    /**
     * Texture mipmapping count.
     */
    uint32_t mipmapCount();
    
    /**
     * Texture format.
     */
    wgpu::TextureFormat format();
    
    wgpu::Texture& texture();
    
public:
    wgpu::TextureViewDimension textureViewDimension();
    
    void setTextureViewDimension(wgpu::TextureViewDimension dim);
    
    virtual wgpu::TextureView textureView() = 0;
    
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
     * Filter mode for texture.
     */
    wgpu::FilterMode mipmapFilter();
    
    void setMipmapFilter(wgpu::FilterMode value);
    
    /**
     * Anisotropic level for texture.
     */
    uint16_t anisoLevel();
    
    void setAnisoLevel(uint16_t value);
    
    wgpu::CompareFunction compareFunction();
    
    void setCompareFunction(wgpu::CompareFunction function);
    
    wgpu::Sampler& sampler();
    
protected:
    uint32_t _getMipmapCount(bool mipmap);
    
    wgpu::ImageCopyBuffer
    _createImageCopyBuffer(wgpu::Buffer buffer,
                           uint64_t offset,
                           uint32_t bytesPerRow,
                           uint32_t rowsPerImage = wgpu::kCopyStrideUndefined);
    
    wgpu::ImageCopyTexture
    _createImageCopyTexture(uint32_t level,
                            wgpu::Origin3D origin,
                            wgpu::TextureAspect aspect = wgpu::TextureAspect::All);
    
    wgpu::TextureDataLayout
    _createTextureDataLayout(uint64_t offset,
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

}

#endif /* sampler_texture_hpp */
