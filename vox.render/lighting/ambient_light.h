//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "vox.math/matrix4x4.h"
#include "vox.math/spherical_harmonics3.h"
#include "vox.render/scene_forward.h"
#include "vox.render/texture/sampled_texture.h"

namespace vox {
/**
 * Diffuse mode.
 */
enum class DiffuseMode {
    /** Solid color mode. */
    SolidColor,

    /**
     * SH mode
     * @remarks
     * Use SH3 to represent irradiance environment maps efficiently, allowing for interactive rendering of diffuse
     * objects under distant illumination.
     */
    SphericalHarmonics
};

/**
 * Ambient light.
 */
class AmbientLight {
public:
    struct alignas(16) EnvMapLight {
        Vector3F diffuse = Vector3F(0.212, 0.227, 0.259);
        uint32_t mipMapLevel = 0;
        float diffuseIntensity = 1.0;
        float specularIntensity = 1.0;
    };

    AmbientLight();

    void setScene(Scene *value);

    /**
     * Diffuse mode of ambient light.
     */
    DiffuseMode diffuseMode();

    void setDiffuseMode(DiffuseMode value);

    /**
     * Diffuse reflection solid color.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SolidColor`.
     */
    [[nodiscard]] Color diffuseSolidColor() const;

    void setDiffuseSolidColor(const Color &value);

    /**
     * Diffuse reflection spherical harmonics 3.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SphericalHarmonics`.
     */
    const SphericalHarmonics3 &diffuseSphericalHarmonics();

    void setDiffuseSphericalHarmonics(const SphericalHarmonics3 &value);

    /**
     * Diffuse reflection intensity.
     */
    [[nodiscard]] float diffuseIntensity() const;

    void setDiffuseIntensity(float value);

public:
    /**
     * Whether to decode from specularTexture with RGBM format.
     */
    [[nodiscard]] bool specularTextureDecodeRGBM() const;

    void setSpecularTextureDecodeRGBM(bool value);

    /**
     * Specular reflection texture.
     * @remarks This texture must be baked from MetalLoader::createSpecularTexture
     */
    std::shared_ptr<SampledTexture> specularTexture();

    void setSpecularTexture(const std::shared_ptr<SampledTexture> &value);

    /**
     * Specular reflection intensity.
     */
    [[nodiscard]] float specularIntensity() const;

    void setSpecularIntensity(float value);

private:
    static std::array<float, 27> _preComputeSH(const SphericalHarmonics3 &sh);

    EnvMapLight _envMapLight;
    static const std::string _envMapProperty;

    SphericalHarmonics3 _diffuseSphericalHarmonics;
    std::array<float, 27> _shArray{};
    static const std::string _diffuseSHProperty;

    bool _specularTextureDecodeRGBM{false};
    std::shared_ptr<SampledTexture> _specularReflection{nullptr};
    static const std::string _specularTextureProperty;
    static const std::string _specularSamplerProperty;

    Scene *_scene{};
    DiffuseMode _diffuseMode = DiffuseMode::SolidColor;
};

}  // namespace vox