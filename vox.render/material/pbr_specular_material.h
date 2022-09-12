//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/pbr_base_material.h"

namespace vox {
/**
 * PBR (Specular-Glossiness Workflow) Material.
 */
class PBRSpecularMaterial : public PBRBaseMaterial {
public:
    struct alignas(16) PBRSpecularData {
        Vector3F specularColor = Vector3F(1, 1, 1);
        float glossiness = 1.f;
    };

    /**
     * Specular color.
     */
    [[nodiscard]] Color specularColor() const;

    void setSpecularColor(const Color& newValue);

    /**
     * Glossiness.
     */
    [[nodiscard]] float glossiness() const;

    void setGlossiness(float newValue);

    /**
     * Specular glossiness texture.
     * @remarks RGB is specular, A is glossiness
     */
    [[nodiscard]] SampledTexture2DPtr specularGlossinessTexture() const;

    void setSpecularGlossinessTexture(const SampledTexture2DPtr& newValue);

    /**
     * Create a pbr specular-glossiness workflow material instance.
     */
    explicit PBRSpecularMaterial(wgpu::Device& device, const std::string& name = "");

private:
    PBRSpecularData _pbrSpecularData;
    static const std::string _pbrSpecularProp;

    SampledTexture2DPtr _specularGlossinessTexture{nullptr};
    static const std::string _specularGlossinessTextureProp;
    static const std::string _specularGlossinessSamplerProp;
};

}  // namespace vox