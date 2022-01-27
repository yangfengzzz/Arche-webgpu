//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef pbr_base_material_hpp
#define pbr_base_material_hpp

#include "base_material.h"
#include "color.h"
#include "vector4.h"
#include "texture/sampled_texture2d.h"

namespace vox {
/**
 * PBR (Physically-Based Rendering) Material.
 */
class PBRBaseMaterial : public BaseMaterial {
public:
    /**
     * Base color.
     */
    const Color& baseColor();
    
    void setBaseColor(const Color &newValue);
    
    /**
     * Base texture.
     */
    SampledTexture2DPtr baseTexture();
    
    void setBaseTexture(SampledTexture2DPtr newValue);
    
    /**
     * Normal texture.
     */
    SampledTexture2DPtr normalTexture();
    
    void setNormalTexture(SampledTexture2DPtr newValue);
    
    /**
     * Normal texture intensity.
     */
    float normalTextureIntensity();
    
    void setNormalTextureIntensity(float newValue);
    
    /**
     * Emissive color.
     */
    const Color& emissiveColor();
    
    void setEmissiveColor(const Color &newValue);
    
    /**
     * Emissive texture.
     */
    SampledTexture2DPtr emissiveTexture();
    
    void setEmissiveTexture(SampledTexture2DPtr newValue);
    
    /**
     * Occlusion texture.
     */
    SampledTexture2DPtr occlusionTexture();
    
    void setOcclusionTexture(SampledTexture2DPtr newValue);
    
    /**
     * Occlusion texture intensity.
     */
    float occlusionTextureIntensity();
    
    void setOcclusionTextureIntensity(float newValue);
    
    /**
     * Tiling and offset of main textures.
     */
    const Vector4F& tilingOffset();
    
    void setTilingOffset(const Vector4F &newValue);
    
protected:
    /**
     * Create a pbr base material instance.
     */
    explicit PBRBaseMaterial(wgpu::Device& device);
    
private:
    ShaderProperty _tilingOffsetProp;
    ShaderProperty _normalTextureIntensityProp;
    ShaderProperty _occlusionTextureIntensityProp;
    
    ShaderProperty _baseColorProp;
    ShaderProperty _emissiveColorProp;
    
    ShaderProperty _baseTextureProp;
    ShaderProperty _normalTextureProp;
    ShaderProperty _emissiveTextureProp;
    ShaderProperty _occlusionTextureProp;
    
    Color _baseColor = Color(1, 1, 1, 1);
    SampledTexture2DPtr _baseTexture{nullptr};
    SampledTexture2DPtr _normalTexture{nullptr};
    float _normalTextureIntensity = 1.f;
    Color _emissiveColor = Color(0, 0, 0, 1);
    SampledTexture2DPtr _emissiveTexture{nullptr};
    SampledTexture2DPtr _occlusionTexture{nullptr};
    float _occlusionTextureIntensity = 1.f;
    Vector4F _tilingOffset = Vector4F(1, 1, 0, 0);
};

}

#endif /* pbr_base_material_hpp */
