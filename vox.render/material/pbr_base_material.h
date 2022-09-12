//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.math/vector3.h"
#include "vox.math/vector4.h"
#include "vox.render/material/base_material.h"
#include "vox.render/material/enums/texture_coordinate.h"

namespace vox {
/**
 * PBR (Physically-Based Rendering) Material.
 */
class PBRBaseMaterial : public BaseMaterial {
public:
    struct alignas(16) PBRBaseData {
        Color baseColor = Color(1, 1, 1, 1);

        Vector3F emissiveColor = Vector3F(0, 0, 0);
        float normalTextureIntensity = 1.f;

        float occlusionTextureIntensity = 1.f;
        TextureCoordinate occlusionTextureCoord = TextureCoordinate::UV0;
        float clearCoat = 0;
        float clearCoatRoughness = 0;
    };

    /**
     * Base color.
     */
    [[nodiscard]] const Color& baseColor() const;

    void setBaseColor(const Color& newValue);

    /**
     * Base texture.
     */
    [[nodiscard]] SampledTexture2DPtr baseTexture() const;

    void setBaseTexture(const SampledTexture2DPtr& newValue);

    /**
     * Normal texture.
     */
    [[nodiscard]] SampledTexture2DPtr normalTexture() const;

    void setNormalTexture(const SampledTexture2DPtr& newValue);

    /**
     * Normal texture intensity.
     */
    [[nodiscard]] float normalTextureIntensity() const;

    void setNormalTextureIntensity(float newValue);

    /**
     * Emissive color.
     */
    [[nodiscard]] Color emissiveColor() const;

    void setEmissiveColor(const Color& newValue);

    /**
     * Emissive texture.
     */
    [[nodiscard]] SampledTexture2DPtr emissiveTexture() const;

    void setEmissiveTexture(const SampledTexture2DPtr& newValue);

    /**
     * Occlusion texture.
     */
    [[nodiscard]] SampledTexture2DPtr occlusionTexture() const;

    void setOcclusionTexture(const SampledTexture2DPtr& newValue);

    /**
     * Occlusion texture intensity.
     */
    [[nodiscard]] float occlusionTextureIntensity() const;

    void setOcclusionTextureIntensity(float newValue);

    /**
     * Occlusion texture uv coordinate.
     * @remarks Must be UV0 or UV1.
     */
    [[nodiscard]] TextureCoordinate occlusionTextureCoord() const;

    void setOcclusionTextureCoord(TextureCoordinate value);

    /**
     * The clearCoat layer intensity, default 0.
     */
    [[nodiscard]] float clearCoat() const;

    void setClearCoat(float value);

    /**
     * The clearCoat layer intensity texture.
     */
    [[nodiscard]] SampledTexture2DPtr clearCoatTexture() const;

    void setClearCoatTexture(const SampledTexture2DPtr& value);

    /**
     * The clearCoat layer roughness, default 0.
     */
    [[nodiscard]] float clearCoatRoughness() const;

    void setClearCoatRoughness(float value);

    /**
     * The clearCoat layer roughness texture.
     */
    [[nodiscard]] SampledTexture2DPtr clearCoatRoughnessTexture() const;

    void setClearCoatRoughnessTexture(const SampledTexture2DPtr& value);

    /**
     * The clearCoat normal map texture.
     */
    [[nodiscard]] SampledTexture2DPtr clearCoatNormalTexture() const;

    void setClearCoatNormalTexture(const SampledTexture2DPtr& value);

protected:
    /**
     * Create a pbr base material instance.
     */
    explicit PBRBaseMaterial(wgpu::Device& device, const std::string& name);

private:
    PBRBaseData _pbrBaseData;
    static const std::string _pbrBaseProp;

    SampledTexture2DPtr _occlusionTexture{nullptr};
    static const std::string _occlusionTextureProp;
    static const std::string _occlusionSamplerProp;

    SampledTexture2DPtr _clearCoatTexture{nullptr};
    static const std::string _clearCoatTextureProp;
    static const std::string _clearCoatSamplerProp;

    SampledTexture2DPtr _clearCoatRoughnessTexture{nullptr};
    static const std::string _clearCoatRoughnessTextureProp;
    static const std::string _clearCoatRoughnessSamplerProp;

    SampledTexture2DPtr _clearCoatNormalTexture{nullptr};
    static const std::string _clearCoatNormalTextureProp;
    static const std::string _clearCoatNormalSamplerProp;
};

}  // namespace vox