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
 * PBR (Physically-Based Rendering) Material.
 */
class PBRBaseMaterial : public BaseMaterial {
public:
    struct alignas(16) PBRBaseData {
        Color baseColor = Color(1, 1, 1, 1);
        Color emissiveColor = Color(0, 0, 0, 1);
        float normalTextureIntensity = 1.f;
        float occlusionTextureIntensity = 1.f;
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
     * Normal texture.
     */
    SampledTexture2DPtr normalTexture();

    void setNormalTexture(const SampledTexture2DPtr& newValue);

    /**
     * Normal texture intensity.
     */
    [[nodiscard]] float normalTextureIntensity() const;

    void setNormalTextureIntensity(float newValue);

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
     * Occlusion texture.
     */
    SampledTexture2DPtr occlusionTexture();

    void setOcclusionTexture(const SampledTexture2DPtr& newValue);

    /**
     * Occlusion texture intensity.
     */
    [[nodiscard]] float occlusionTextureIntensity() const;

    void setOcclusionTextureIntensity(float newValue);

protected:
    /**
     * Create a pbr base material instance.
     */
    explicit PBRBaseMaterial(wgpu::Device& device, const std::string& name);

private:
    PBRBaseData _pbrBaseData;
    const std::string _pbrBaseProp;

    SampledTexture2DPtr _baseTexture{nullptr};
    const std::string _baseTextureProp;
    const std::string _baseSamplerProp;

    SampledTexture2DPtr _normalTexture{nullptr};
    const std::string _normalTextureProp;
    const std::string _normalSamplerProp;

    SampledTexture2DPtr _emissiveTexture{nullptr};
    const std::string _emissiveTextureProp;
    const std::string _emissiveSamplerProp;

    SampledTexture2DPtr _occlusionTexture{nullptr};
    const std::string _occlusionTextureProp;
    const std::string _occlusionSamplerProp;
};

}  // namespace vox