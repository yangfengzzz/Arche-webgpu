//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef blinn_phong_material_hpp
#define blinn_phong_material_hpp

#include "base_material.h"
#include "color.h"
#include "vector4.h"
#include "texture/sampled_texture2d.h"

namespace vox {
/**
 * Blinn-phong Material.
 */
class BlinnPhongMaterial : public BaseMaterial {
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
     * Specular color.
     */
    const Color& specularColor();
    
    void setSpecularColor(const Color &newValue);
    
    /**
     * Specular texture.
     */
    SampledTexture2DPtr specularTexture();
    
    void setSpecularTexture(SampledTexture2DPtr newValue);
    
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
     * Normal texture.
     */
    SampledTexture2DPtr normalTexture();
    
    void setNormalTexture(SampledTexture2DPtr newValue);
    
    /**
     * Normal texture intensity.
     */
    float normalIntensity();
    
    void setNormalIntensity(float newValue);
    
    /**
     * Set the specular reflection coefficient, the larger the value, the more convergent the specular reflection effect.
     */
    float shininess();
    
    void setShininess(float newValue);
    
    /**
     * Tiling and offset of main textures.
     */
    const Vector4F& tilingOffset();
    
    void setTilingOffset(const Vector4F &newValue);
    
    explicit BlinnPhongMaterial(wgpu::Device& device);
    
private:
    ShaderProperty _diffuseColorProp;
    ShaderProperty _specularColorProp;
    ShaderProperty _emissiveColorProp;
    ShaderProperty _tilingOffsetProp;
    ShaderProperty _shininessProp;
    ShaderProperty _normalIntensityProp;
    
    ShaderProperty _baseTextureProp;
    ShaderProperty _baseSamplerProp;
    
    ShaderProperty _specularTextureProp;
    ShaderProperty _specularSamplerProp;
    
    ShaderProperty _emissiveTextureProp;
    ShaderProperty _emissiveSamplerProp;
    
    ShaderProperty _normalTextureProp;
    ShaderProperty _normalSamplerProp;
    
    Color _baseColor = Color(1, 1, 1, 1);
    SampledTexture2DPtr _baseTexture{nullptr};
    Color _specularColor = Color(1, 1, 1, 1);
    SampledTexture2DPtr _specularTexture{nullptr};
    Color _emissiveColor = Color(0, 0, 0, 1);
    SampledTexture2DPtr _emissiveTexture{nullptr};
    SampledTexture2DPtr _normalTexture{nullptr};
    float _normalIntensity = 1.f;
    float _shininess = 16.f;
    Vector4F _tilingOffset = Vector4F(1, 1, 0, 0);
};


}

#endif /* blinn_phong_material_hpp */
