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
    struct PBRBaseData {
        Color baseColor = Color(1, 1, 1, 1);
        Color emissiveColor = Color(0, 0, 0, 1);
        float normalTextureIntensity = 1.f;
        float occlusionTextureIntensity = 1.f;
        float _pad1, _pad2;
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
    
protected:
    /**
     * Create a pbr base material instance.
     */
    explicit PBRBaseMaterial(wgpu::Device& device, Shader *shader);
    
private:    
    PBRBaseData _pbrBaseData;
    ShaderProperty _pbrBaseProp;
    
    SampledTexture2DPtr _baseTexture{nullptr};
    ShaderProperty _baseTextureProp;
    ShaderProperty _baseSamplerProp;
    
    SampledTexture2DPtr _normalTexture{nullptr};
    ShaderProperty _normalTextureProp;
    ShaderProperty _normalSamplerProp;

    SampledTexture2DPtr _emissiveTexture{nullptr};
    ShaderProperty _emissiveTextureProp;
    ShaderProperty _emissiveSamplerProp;

    SampledTexture2DPtr _occlusionTexture{nullptr};
    ShaderProperty _occlusionTextureProp;
    ShaderProperty _occlusionSamplerProp;
};

}

#endif /* pbr_base_material_hpp */
