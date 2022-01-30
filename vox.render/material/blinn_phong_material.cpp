//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "blinn_phong_material.h"

namespace vox {
const Color& BlinnPhongMaterial::baseColor() {
    return _baseColor;
}

void BlinnPhongMaterial::setBaseColor(const Color &newValue) {
    _baseColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_diffuseColorProp, newValue);
}

SampledTexture2DPtr BlinnPhongMaterial::baseTexture() {
    return _baseTexture;
}

void BlinnPhongMaterial::setBaseTexture(SampledTexture2DPtr newValue) {
    _baseTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_baseTextureProp,
                                 BlinnPhongMaterial::_baseSamplerProp, newValue);
    
    if (newValue) {
        shaderData.enableMacro(HAS_DIFFUSE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_DIFFUSE_TEXTURE);
    }
}

const Color& BlinnPhongMaterial::specularColor() {
    return _specularColor;
}

void BlinnPhongMaterial::setSpecularColor(const Color &newValue) {
    _specularColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_specularColorProp, newValue);
}

SampledTexture2DPtr BlinnPhongMaterial::specularTexture() {
    return _specularTexture;
}

void BlinnPhongMaterial::setSpecularTexture(SampledTexture2DPtr newValue) {
    _specularTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_specularTextureProp,
                                 BlinnPhongMaterial::_specularSamplerProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_SPECULAR_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_SPECULAR_TEXTURE);
    }
}

const Color& BlinnPhongMaterial::emissiveColor() {
    return _emissiveColor;
}

void BlinnPhongMaterial::setEmissiveColor(const Color &newValue) {
    _emissiveColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_emissiveColorProp, newValue);
}

SampledTexture2DPtr BlinnPhongMaterial::emissiveTexture() {
    return _emissiveTexture;
}

void BlinnPhongMaterial::BlinnPhongMaterial::setEmissiveTexture(SampledTexture2DPtr newValue) {
    _emissiveTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_emissiveTextureProp,
                                 BlinnPhongMaterial::_emissiveSamplerProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_EMISSIVE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_EMISSIVE_TEXTURE);
    }
}

SampledTexture2DPtr BlinnPhongMaterial::normalTexture() {
    return _normalTexture;
}

void BlinnPhongMaterial::setNormalTexture(SampledTexture2DPtr newValue) {
    _normalTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_normalTextureProp,
                                 BlinnPhongMaterial::_normalSamplerProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_NORMAL_TEXTURE);
    }
}

float BlinnPhongMaterial::normalIntensity() {
    return _normalIntensity;
}

void BlinnPhongMaterial::setNormalIntensity(float newValue) {
    _normalIntensity = newValue;
    shaderData.setData(BlinnPhongMaterial::_normalIntensityProp, newValue);
}

float BlinnPhongMaterial::shininess() {
    return _shininess;
}

void BlinnPhongMaterial::setShininess(float newValue) {
    _shininess = newValue;
    shaderData.setData(BlinnPhongMaterial::_shininessProp, newValue);
}

const Vector4F& BlinnPhongMaterial::tilingOffset() {
    return _tilingOffset;
}

void BlinnPhongMaterial::setTilingOffset(const Vector4F &newValue) {
    _tilingOffset = newValue;
    shaderData.setData(BlinnPhongMaterial::_tilingOffsetProp, newValue);
}

BlinnPhongMaterial::BlinnPhongMaterial(wgpu::Device& device) :
BaseMaterial(device, Shader::find("blinn-phong")),
_diffuseColorProp(Shader::createProperty("u_diffuseColor", ShaderDataGroup::Material)),
_specularColorProp(Shader::createProperty("u_specularColor", ShaderDataGroup::Material)),
_emissiveColorProp(Shader::createProperty("u_emissiveColor", ShaderDataGroup::Material)),
_tilingOffsetProp(Shader::createProperty("u_tilingOffset", ShaderDataGroup::Material)),
_shininessProp(Shader::createProperty("u_shininess", ShaderDataGroup::Material)),
_normalIntensityProp(Shader::createProperty("u_normalIntensity", ShaderDataGroup::Material)),

_baseTextureProp(Shader::createProperty("u_diffuseTexture", ShaderDataGroup::Material)),
_baseSamplerProp(Shader::createProperty("u_diffuseSampler", ShaderDataGroup::Material)),

_specularTextureProp(Shader::createProperty("u_specularTexture", ShaderDataGroup::Material)),
_specularSamplerProp(Shader::createProperty("u_specularSampler", ShaderDataGroup::Material)),

_emissiveTextureProp(Shader::createProperty("u_emissiveTexture", ShaderDataGroup::Material)),
_emissiveSamplerProp(Shader::createProperty("u_emissiveSampler", ShaderDataGroup::Material)),

_normalTextureProp(Shader::createProperty("u_normalTexture", ShaderDataGroup::Material)),
_normalSamplerProp(Shader::createProperty("u_normalSampler", ShaderDataGroup::Material)){

    shaderData.enableMacro(NEED_WORLDPOS);
    shaderData.enableMacro(NEED_TILINGOFFSET);
    
    shaderData.setData(_diffuseColorProp, _baseColor);
    shaderData.setData(_specularColorProp, _specularColor);
    shaderData.setData(_emissiveColorProp, _emissiveColor);
    shaderData.setData(_tilingOffsetProp, _tilingOffset);
    shaderData.setData(_shininessProp, _shininess);
    shaderData.setData(_normalIntensityProp, _normalIntensity);
}

}
