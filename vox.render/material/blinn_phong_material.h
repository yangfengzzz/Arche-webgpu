//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.math/vector4.h"
#include "vox.render/material/base_material.h"
#include "vox.render/texture/sampled_texture2d.h"

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
        float _pad1, _pad2;  // align
    };

    /**
     * Base color.
     */
    [[nodiscard]] const Color& baseColor() const;

    void setBaseColor(const Color& newValue);

    /**
     * Base texture.
     */
    SampledTexture2DPtr baseTexture();

    void setBaseTexture(const SampledTexture2DPtr& newValue);

    /**
     * Specular color.
     */
    [[nodiscard]] const Color& specularColor() const;

    void setSpecularColor(const Color& newValue);

    /**
     * Specular texture.
     */
    SampledTexture2DPtr specularTexture();

    void setSpecularTexture(const SampledTexture2DPtr& newValue);

    /**
     * Emissive color.
     */
    [[nodiscard]] const Color& emissiveColor() const;

    void setEmissiveColor(const Color& newValue);

    /**
     * Emissive texture.
     */
    SampledTexture2DPtr emissiveTexture();

    void setEmissiveTexture(const SampledTexture2DPtr& newValue);

    /**
     * Normal texture.
     */
    SampledTexture2DPtr normalTexture();

    void setNormalTexture(const SampledTexture2DPtr& newValue);

    /**
     * Normal texture intensity.
     */
    [[nodiscard]] float normalIntensity() const;

    void setNormalIntensity(float newValue);

    /**
     * Set the specular reflection coefficient, the larger the value, the more convergent the specular reflection
     * effect.
     */
    [[nodiscard]] float shininess() const;

    void setShininess(float newValue);

    explicit BlinnPhongMaterial(wgpu::Device& device);

private:
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

}  // namespace vox