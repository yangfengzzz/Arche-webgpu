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
    struct BlinnPhongData {
        Color baseColor = Color(1, 1, 1, 1);
        Color specularColor = Color(1, 1, 1, 1);
        Color emissiveColor = Color(0, 0, 0, 1);
        float normalIntensity = 1.f;
        float shininess = 16.f;
        float _pad1, _pad2; // align
    };
    
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
    Vector4F _tilingOffset = Vector4F(1, 1, 0, 0);
    ShaderProperty _tilingOffsetProp;

    BlinnPhongData _blinnPhongData;
    ShaderProperty _blinnPhongProp;
    
    SampledTexture2DPtr _baseTexture{nullptr};
    ShaderProperty _baseTextureProp;
    ShaderProperty _baseSamplerProp;
    
    SampledTexture2DPtr _specularTexture{nullptr};
    ShaderProperty _specularTextureProp;
    ShaderProperty _specularSamplerProp;
    
    SampledTexture2DPtr _emissiveTexture{nullptr};
    ShaderProperty _emissiveTextureProp;
    ShaderProperty _emissiveSamplerProp;
    
    SampledTexture2DPtr _normalTexture{nullptr};
    ShaderProperty _normalTextureProp;
    ShaderProperty _normalSamplerProp;
};


}

#endif /* blinn_phong_material_hpp */
