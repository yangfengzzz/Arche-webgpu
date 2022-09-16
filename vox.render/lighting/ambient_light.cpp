//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/ambient_light.h"

#include "vox.render/scene.h"
#include "vox.render/shader/internal_variant_name.h"

namespace vox {
const std::string AmbientLight::_envMapProperty = "u_envMapLight";
const std::string AmbientLight::_diffuseSHProperty = "u_env_sh";
const std::string AmbientLight::_specularTextureProperty = "u_env_specularTexture";
const std::string AmbientLight::_specularSamplerProperty = "u_env_specularSampler";

AmbientLight::AmbientLight() = default;

void AmbientLight::setScene(Scene *value) {
    _scene = value;
    if (!value) return;

    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
}

DiffuseMode AmbientLight::diffuseMode() { return _diffuseMode; }

void AmbientLight::setDiffuseMode(DiffuseMode value) {
    _diffuseMode = value;
    if (!_scene) return;

    if (value == DiffuseMode::SphericalHarmonics) {
        _scene->shaderData.addDefine(HAS_SH);
    } else {
        _scene->shaderData.removeDefine(HAS_SH);
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

float AmbientLight::diffuseIntensity() const { return _envMapLight.diffuseIntensity; }

void AmbientLight::setDiffuseIntensity(float value) {
    _envMapLight.diffuseIntensity = value;
    if (!_scene) return;

    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
}

// MARK: - Specular
bool AmbientLight::specularTextureDecodeRGBM() const { return _specularTextureDecodeRGBM; }

void AmbientLight::setSpecularTextureDecodeRGBM(bool value) {
    this->_specularTextureDecodeRGBM = value;
    if (value) {
        _scene->shaderData.addDefine(DECODE_ENV_RGBM);
    } else {
        _scene->shaderData.removeDefine(DECODE_ENV_RGBM);
    }
}

std::shared_ptr<Image> AmbientLight::specularTexture() { return _specularReflection; }

void AmbientLight::setSpecularTexture(const std::shared_ptr<Image> &value) {
    _specularReflection = value;
    if (!_scene) return;

    auto &shaderData = _scene->shaderData;

    if (value) {
        shaderData.setImageView(AmbientLight::_specularTextureProperty, AmbientLight::_specularSamplerProperty,
                                _specularReflection->getImageView(wgpu::TextureViewDimension::Cube));
        _envMapLight.mipMapLevel = static_cast<uint32_t>(value->getTexture().GetMipLevelCount() - 1);
        _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
        shaderData.addDefine(HAS_SPECULAR_ENV);
    } else {
        shaderData.removeDefine(HAS_SPECULAR_ENV);
    }
}

void AmbientLight::setSpecularSampler(const wgpu::SamplerDescriptor& newValue) {
    _scene->shaderData.setSampler(AmbientLight::_specularSamplerProperty, newValue);
}

float AmbientLight::specularIntensity() const { return _envMapLight.specularIntensity; }

void AmbientLight::setSpecularIntensity(float value) {
    _envMapLight.specularIntensity = value;
    if (!_scene) return;

    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
}

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
