//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef pbr_specular_material_hpp
#define pbr_specular_material_hpp

#include "pbr_base_material.h"

namespace vox {
/**
 * PBR (Specular-Glossiness Workflow) Material.
 */
class PBRSpecularMaterial : public PBRBaseMaterial {
public:
    /**
     * Specular color.
     */
    const Color& specularColor();
    
    void setSpecularColor(const Color &newValue);
    
    /**
     * Glossiness.
     */
    float glossiness();
    
    void setGlossiness(float newValue);
    
    /**
     * Specular glossiness texture.
     * @remarks RGB is specular, A is glossiness
     */
    SampledTexture2DPtr specularGlossinessTexture();
    
    void setSpecularGlossinessTexture(SampledTexture2DPtr newValue);
    
    /**
     * Create a pbr specular-glossiness workflow material instance.
     */
    explicit PBRSpecularMaterial(wgpu::Device& device);
    
private:
    ShaderProperty _glossinessProp;
    ShaderProperty _specularColorProp;
    ShaderProperty _specularGlossinessTextureProp;
    
    Color _specularColor = Color(1, 1, 1, 1);
    float _glossiness = 1.f;;
    SampledTexture2DPtr _specularGlossinessTexture{nullptr};
};

}

#endif /* pbr_specular_material_hpp */
