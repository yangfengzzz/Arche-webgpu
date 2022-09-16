//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.math/vector4.h"
#include "vox.render/material/base_material.h"
#include "vox.render/image.h"

namespace vox {
/**
 * Blinn-phong Material.
 */
class BlinnPhongMaterial : public BaseMaterial {
public:
    struct alignas(16) BlinnPhongData {
        Color baseColor = Color(1, 1, 1, 1);
        Color specularColor = Color(1, 1, 1, 1);
        Color emissiveColor = Color(0, 0, 0, 1);
        float normalIntensity = 1.f;
        float shininess = 16.f;
    };

    /**
     * Base color.
     */
    [[nodiscard]] const Color& baseColor() const;

    void setBaseColor(const Color& newValue);

    /**
     * Base texture.
     */
    [[nodiscard]] std::shared_ptr<Image> baseTexture() const;

    void setBaseTexture(const std::shared_ptr<Image>& newValue);

    void setBaseSampler(const wgpu::SamplerDescriptor& newValue);

    /**
     * Specular color.
     */
    [[nodiscard]] const Color& specularColor() const;

    void setSpecularColor(const Color& newValue);

    /**
     * Specular texture.
     */
    [[nodiscard]] std::shared_ptr<Image> specularTexture() const;

    void setSpecularTexture(const std::shared_ptr<Image>& newValue);

    void setSpecularSampler(const wgpu::SamplerDescriptor& newValue);

    /**
     * Emissive color.
     */
    [[nodiscard]] const Color& emissiveColor() const;

    void setEmissiveColor(const Color& newValue);

    /**
     * Emissive texture.
     */
    [[nodiscard]] std::shared_ptr<Image> emissiveTexture() const;

    void setEmissiveTexture(const std::shared_ptr<Image>& newValue);

    void setEmissiveSampler(const wgpu::SamplerDescriptor& newValue);

    /**
     * Normal texture.
     */
    [[nodiscard]] std::shared_ptr<Image> normalTexture() const;

    void setNormalTexture(const std::shared_ptr<Image>& newValue);

    void setNormalSampler(const wgpu::SamplerDescriptor& newValue);

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

    explicit BlinnPhongMaterial(wgpu::Device& device, const std::string& name = "");

private:
    BlinnPhongData _blinnPhongData;
    static const std::string _blinnPhongProp;

    std::shared_ptr<Image> _specularTexture{nullptr};
    static const std::string _specularTextureProp;
    static const std::string _specularSamplerProp;
};

}  // namespace vox