//
//  ambient_light.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

#include "ambient_light.h"
#include "shader/shader.h"
#include "scene.h"

namespace vox {
AmbientLight::AmbientLight(Scene *value):
_envMapProperty(Shader::createProperty("u_envMapLight", ShaderDataGroup::Scene)),
_diffuseSHProperty(Shader::createProperty("u_env_sh", ShaderDataGroup::Scene)),
_diffuseTextureProperty(Shader::createProperty("u_env_diffuseTexture", ShaderDataGroup::Scene)),
_specularTextureProperty(Shader::createProperty("u_env_specularTexture", ShaderDataGroup::Scene)),
_brdfTextureProperty(Shader::createProperty("u_env_brdfTexture", ShaderDataGroup::Scene)) {
    _scene = value;
    if (!value) return;
    
    _envMapLight.diffuse = Vector3F(0.212, 0.227, 0.259);
    _envMapLight.diffuseIntensity = 1.0;
    _envMapLight.specularIntensity = 1.0;
    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
    // _scene->shaderData.setData(AmbientLight::_brdfTextureProperty,
    //                           value->engine()->resourceLoader()->createBRDFLookupTable());
}

DiffuseMode AmbientLight::diffuseMode() {
    return _diffuseMode;
}

void AmbientLight::setDiffuseMode(DiffuseMode value) {
    _diffuseMode = value;
    if (!_scene) return;
    
    switch (value) {
        case DiffuseMode::SphericalHarmonics:
            _scene->shaderData.disableMacro(HAS_DIFFUSE_ENV);
            _scene->shaderData.enableMacro(HAS_SH);
            break;
            
        case DiffuseMode::Texture:
            _scene->shaderData.disableMacro(HAS_SH);
            _scene->shaderData.enableMacro(HAS_DIFFUSE_ENV);
            break;
            
        default:
            break;
    }
}

Color AmbientLight::diffuseSolidColor() {
    return Color(_envMapLight.diffuse.x, _envMapLight.diffuse.y, _envMapLight.diffuse.z);
}

void AmbientLight::setDiffuseSolidColor(const Color &value) {
    _envMapLight.diffuse = Vector3F(value.r, value.g, value.b);
    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
}

const SphericalHarmonics3 &AmbientLight::diffuseSphericalHarmonics() {
    return _diffuseSphericalHarmonics;
}

void AmbientLight::setDiffuseSphericalHarmonics(const SphericalHarmonics3 &value) {
    _diffuseSphericalHarmonics = value;
    if (!_scene) return;
    
    _scene->shaderData.setData(AmbientLight::_diffuseSHProperty, _preComputeSH(value));
}

std::shared_ptr<SampledTexture> AmbientLight::diffuseTexture() {
    return _diffuseTexture;
}

void AmbientLight::setDiffuseTexture(std::shared_ptr<SampledTexture> value) {
    _diffuseTexture = value;
    if (!_scene) return;
    
    auto &shaderData = _scene->shaderData;
    
    if (value) {
        shaderData.setData(AmbientLight::_diffuseTextureProperty, _diffuseTexture);
        shaderData.enableMacro(HAS_DIFFUSE_ENV);
    } else {
        shaderData.disableMacro(HAS_DIFFUSE_ENV);
    }
}

float AmbientLight::diffuseIntensity() {
    return _envMapLight.diffuseIntensity;
}

void AmbientLight::setDiffuseIntensity(float value) {
    _envMapLight.diffuseIntensity = value;
    if (!_scene) return;
    
    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
}

//MARK: - Specular
bool AmbientLight::specularTextureDecodeRGBM() {
    return _specularTextureDecodeRGBM;
}

void AmbientLight::setSpecularTextureDecodeRGBM(bool value) {
    
}

std::shared_ptr<SampledTexture> AmbientLight::specularTexture() {
    return _specularReflection;
}

void AmbientLight::setSpecularTexture(std::shared_ptr<SampledTexture> value) {
    _specularReflection = value;
    if (!_scene) return;
    
    auto &shaderData = _scene->shaderData;
    
    if (value) {
        shaderData.setData(AmbientLight::_specularTextureProperty, _specularReflection);
        _envMapLight.mipMapLevel = static_cast<int>(value->mipmapCount() - 1);
        _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
        shaderData.enableMacro(HAS_SPECULAR_ENV);
    } else {
        shaderData.disableMacro(HAS_SPECULAR_ENV);
    }
}

float AmbientLight::specularIntensity() {
    return _envMapLight.specularIntensity;
}

void AmbientLight::setSpecularIntensity(float value) {
    _envMapLight.specularIntensity = value;
    if (!_scene) return;
    
    _scene->shaderData.setData(AmbientLight::_envMapProperty, _envMapLight);
}

//MARK: - BRDF Texture
std::shared_ptr<SampledTexture> AmbientLight::brdfTexture() {
    return _brdfLutTexture;
}

void AmbientLight::setBRDFTexture(std::shared_ptr<SampledTexture> value) {
    
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
    std::array<float, 27> out;
    // l0
    out[0] = src[0] * 0.886227; // kernel0 * basis0 = 0.886227
    out[1] = src[1] * 0.886227;
    out[2] = src[2] * 0.886227;
    
    // l1
    out[3] = src[3] * -1.023327; // kernel1 * basis1 = -1.023327;
    out[4] = src[4] * -1.023327;
    out[5] = src[5] * -1.023327;
    out[6] = src[6] * 1.023327; // kernel1 * basis2 = 1.023327
    out[7] = src[7] * 1.023327;
    out[8] = src[8] * 1.023327;
    out[9] = src[9] * -1.023327; // kernel1 * basis3 = -1.023327
    out[10] = src[10] * -1.023327;
    out[11] = src[11] * -1.023327;
    
    // l2
    out[12] = src[12] * 0.858086; // kernel2 * basis4 = 0.858086
    out[13] = src[13] * 0.858086;
    out[14] = src[14] * 0.858086;
    out[15] = src[15] * -0.858086; // kernel2 * basis5 = -0.858086
    out[16] = src[16] * -0.858086;
    out[17] = src[17] * -0.858086;
    out[18] = src[18] * 0.247708; // kernel2 * basis6 = 0.247708
    out[19] = src[19] * 0.247708;
    out[20] = src[20] * 0.247708;
    out[21] = src[21] * -0.858086; // kernel2 * basis7 = -0.858086
    out[22] = src[22] * -0.858086;
    out[23] = src[23] * -0.858086;
    out[24] = src[24] * 0.429042; // kernel2 * basis8 = 0.429042
    out[25] = src[25] * 0.429042;
    out[26] = src[26] * 0.429042;
    
    return out;
}

}
