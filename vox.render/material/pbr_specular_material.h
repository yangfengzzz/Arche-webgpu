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
    struct PBRSpecularData {
        Color specularColor = Color(1, 1, 1, 1);
        float glossiness = 1.f;;
        float _pad1, _pad2, _pad3;
    };
    
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
    PBRSpecularData _pbrSpecularData;
    ShaderProperty _pbrSpecularProp;
    
    SampledTexture2DPtr _specularGlossinessTexture{nullptr};
    ShaderProperty _specularGlossinessTextureProp;
    ShaderProperty _specularGlossinessSamplerProp;
};

}

#endif /* pbr_specular_material_hpp */
