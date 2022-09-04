//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/ambient_light.h"

#include "vox.render/scene.h"
#include "vox.render/shader/internal_variant_name.h"

namespace vox {
AmbientLight::AmbientLight()
    : _envMapProperty("u_envMapLight"),
      _diffuseSHProperty("u_env_sh"),
      _diffuseTextureProperty("u_env_diffuseTexture"),
      _diffuseSamplerProperty("u_env_diffuseSampler"),
      _specularTextureProperty("u_env_specularTexture"),
      _specularSamplerProperty("u_env_specularSampler"),
      _brdfTextureProperty("u_env_brdfTexture"),
      _brdfSamplerProperty("u_env_brdfSampler") {}

void AmbientLight::setScene(Scene *value) {
    _scene = value;
    if (!value) return;

    _envMapLight.diffuse = Vector3F(0.212, 0.227, 0.259);
    _envMapLight.diffuseIntensity = 1.0;
    _envMapLight.specularIntensity = 1.0;
    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
    // _scene->shaderData.setData(AmbientLight::_brdfTextureProperty,
    //                           value->engine()->resourceLoader()->createBRDFLookupTable());
}

DiffuseMode AmbientLight::diffuseMode() { return _diffuseMode; }

void AmbientLight::setDiffuseMode(DiffuseMode value) {
    _diffuseMode = value;
    if (!_scene) return;

    switch (value) {
        case DiffuseMode::SphericalHarmonics:
            _scene->shaderData.removeDefine(HAS_DIFFUSE_ENV);
            _scene->shaderData.addDefine(HAS_SH);
            break;

        case DiffuseMode::Texture:
            _scene->shaderData.removeDefine(HAS_SH);
            _scene->shaderData.addDefine(HAS_DIFFUSE_ENV);
            break;

        default:
            break;
    }
}

Color AmbientLight::diffuseSolidColor() const {
    return {_envMapLight.diffuse.x, _envMapLight.diffuse.y, _envMapLight.diffuse.z};
}

void AmbientLight::setDiffuseSolidColor(const Color &value) {
    _envMapLight.diffuse = Vector3F(value.r, value.g, value.b);
    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
}

const SphericalHarmonics3 &AmbientLight::diffuseSphericalHarmonics() { return _diffuseSphericalHarmonics; }

void AmbientLight::setDiffuseSphericalHarmonics(const SphericalHarmonics3 &value) {
    _diffuseSphericalHarmonics = value;
    if (!_scene) return;

    _scene->shaderData.setData(AmbientLight::_diffuseSHProperty, _preComputeSH(value));
}

std::shared_ptr<SampledTexture> AmbientLight::diffuseTexture() { return _diffuseTexture; }

void AmbientLight::setDiffuseTexture(const std::shared_ptr<SampledTexture> &value) {
    _diffuseTexture = value;
    if (!_scene) return;

    auto &shaderData = _scene->shaderData;

    if (value) {
        shaderData.setSampledTexture(AmbientLight::_diffuseTextureProperty, AmbientLight::_diffuseSamplerProperty,
                                     _diffuseTexture);
        shaderData.removeDefine(HAS_DIFFUSE_ENV);
    } else {
        shaderData.addDefine(HAS_DIFFUSE_ENV);
    }
}

float AmbientLight::diffuseIntensity() const { return _envMapLight.diffuseIntensity; }

void AmbientLight::setDiffuseIntensity(float value) {
    _envMapLight.diffuseIntensity = value;
    if (!_scene) return;

    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
}

// MARK: - Specular
bool AmbientLight::specularTextureDecodeRGBM() const { return _specularTextureDecodeRGBM; }

void AmbientLight::setSpecularTextureDecodeRGBM(bool value) {}

std::shared_ptr<SampledTexture> AmbientLight::specularTexture() { return _specularReflection; }

void AmbientLight::setSpecularTexture(const std::shared_ptr<SampledTexture> &value) {
    _specularReflection = value;
    if (!_scene) return;

    auto &shaderData = _scene->shaderData;

    if (value) {
        shaderData.setSampledTexture(AmbientLight::_specularTextureProperty, AmbientLight::_specularSamplerProperty,
                                     _specularReflection);
        _envMapLight.mipMapLevel = static_cast<uint32_t>(value->mipmapCount() - 1);
        _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
        shaderData.removeDefine(HAS_SPECULAR_ENV);
    } else {
        shaderData.addDefine(HAS_SPECULAR_ENV);
    }
}

float AmbientLight::specularIntensity() const { return _envMapLight.specularIntensity; }

void AmbientLight::setSpecularIntensity(float value) {
    _envMapLight.specularIntensity = value;
    if (!_scene) return;

    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
}

// MARK: - BRDF Texture
std::shared_ptr<SampledTexture> AmbientLight::brdfTexture() { return _brdfLutTexture; }

void AmbientLight::setBRDFTexture(const std::shared_ptr<SampledTexture> &value) {}

std::array<float, 27> AmbientLight::_preComputeSH(const SphericalHarmonics3 &sh) {
    /**
     * Basis constants
     *
     * 0: 1/2 * Math.sqrt(1 / Math.PI)
     *
     * 1: -1/2 * Math.sqrt(3 / Math.PI)
     * 2: 1/2 * Math.sqrt(3 / Math.PI)
     * 3: -1/2 * Math.sqrt(3 / Math.PI)
     *
     * 4: 1/2 * Math.sqrt(15 / Math.PI)
     * 5: -1/2 * Math.sqrt(15 / Math.PI)
     * 6: 1/4 * Math.sqrt(5 / Math.PI)
     * 7: -1/2 * Math.sqrt(15 / Math.PI)
     * 8: 1/4 * Math.sqrt(15 / Math.PI)
     */

    /**
     * Convolution kernel
     *
     * 0: Math.PI
     * 1: (2 * Math.PI) / 3
     * 2: Math.PI / 4
     */

    const auto &src = sh.coefficients();
    std::array<float, 27> out{};
    // l0
    out[0] = src[0] * 0.886227f;  // kernel0 * basis0 = 0.886227
    out[1] = src[1] * 0.886227f;
    out[2] = src[2] * 0.886227f;

    // l1
    out[3] = src[3] * -1.023327f;  // kernel1 * basis1 = -1.023327;
    out[4] = src[4] * -1.023327f;
    out[5] = src[5] * -1.023327f;
    out[6] = src[6] * 1.023327f;  // kernel1 * basis2 = 1.023327
    out[7] = src[7] * 1.023327f;
    out[8] = src[8] * 1.023327f;
    out[9] = src[9] * -1.023327f;  // kernel1 * basis3 = -1.023327
    out[10] = src[10] * -1.023327f;
    out[11] = src[11] * -1.023327f;

    // l2
    out[12] = src[12] * 0.858086f;  // kernel2 * basis4 = 0.858086
    out[13] = src[13] * 0.858086f;
    out[14] = src[14] * 0.858086f;
    out[15] = src[15] * -0.858086f;  // kernel2 * basis5 = -0.858086
    out[16] = src[16] * -0.858086f;
    out[17] = src[17] * -0.858086f;
    out[18] = src[18] * 0.247708f;  // kernel2 * basis6 = 0.247708
    out[19] = src[19] * 0.247708f;
    out[20] = src[20] * 0.247708f;
    out[21] = src[21] * -0.858086f;  // kernel2 * basis7 = -0.858086
    out[22] = src[22] * -0.858086f;
    out[23] = src[23] * -0.858086f;
    out[24] = src[24] * 0.429042f;  // kernel2 * basis8 = 0.429042
    out[25] = src[25] * 0.429042f;
    out[26] = src[26] * 0.429042f;

    return out;
}

}  // namespace vox
