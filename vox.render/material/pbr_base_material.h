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
    [[nodiscard]] std::shared_ptr<Image> baseTexture() const;

    void setBaseTexture(const std::shared_ptr<Image>& newValue);

    void setBaseSampler(const wgpu::SamplerDescriptor& newValue);

    /**
     * Normal texture.
     */
    [[nodiscard]] std::shared_ptr<Image> normalTexture() const;

    void setNormalTexture(const std::shared_ptr<Image>& newValue);

    void setNormalSampler(const wgpu::SamplerDescriptor& newValue);

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
    [[nodiscard]] std::shared_ptr<Image> emissiveTexture() const;

    void setEmissiveTexture(const std::shared_ptr<Image>& newValue);

    void setEmissiveSampler(const wgpu::SamplerDescriptor& newValue);

    /**
     * Occlusion texture.
     */
    [[nodiscard]] std::shared_ptr<Image> occlusionTexture() const;

    void setOcclusionTexture(const std::shared_ptr<Image>& newValue);

    void setOcclusionSampler(const wgpu::SamplerDescriptor& newValue);

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
    [[nodiscard]] std::shared_ptr<Image> clearCoatTexture() const;

    void setClearCoatTexture(const std::shared_ptr<Image>& value);

    void setClearCoatSampler(const wgpu::SamplerDescriptor& newValue);

    /**
     * The clearCoat layer roughness, default 0.
     */
    [[nodiscard]] float clearCoatRoughness() const;

    void setClearCoatRoughness(float value);

    /**
     * The clearCoat layer roughness texture.
     */
    [[nodiscard]] std::shared_ptr<Image> clearCoatRoughnessTexture() const;

    void setClearCoatRoughnessTexture(const std::shared_ptr<Image>& value);

    void setClearCoatRoughnessSampler(const wgpu::SamplerDescriptor& newValue);

    /**
     * The clearCoat normal map texture.
     */
    [[nodiscard]] std::shared_ptr<Image> clearCoatNormalTexture() const;

    void setClearCoatNormalTexture(const std::shared_ptr<Image>& value);

    void setClearCoatNormalSampler(const wgpu::SamplerDescriptor& newValue);

protected:
    /**
     * Create a pbr base material instance.
     */
    explicit PBRBaseMaterial(wgpu::Device& device, const std::string& name);

private:
    PBRBaseData _pbrBaseData;
    static const std::string _pbrBaseProp;

    std::shared_ptr<Image> _occlusionTexture{nullptr};
    static const std::string _occlusionTextureProp;
    static const std::string _occlusionSamplerProp;

    std::shared_ptr<Image> _clearCoatTexture{nullptr};
    static const std::string _clearCoatTextureProp;
    static const std::string _clearCoatSamplerProp;

    std::shared_ptr<Image> _clearCoatRoughnessTexture{nullptr};
    static const std::string _clearCoatRoughnessTextureProp;
    static const std::string _clearCoatRoughnessSamplerProp;

    std::shared_ptr<Image> _clearCoatNormalTexture{nullptr};
    static const std::string _clearCoatNormalTextureProp;
    static const std::string _clearCoatNormalSamplerProp;
};

}  // namespace vox