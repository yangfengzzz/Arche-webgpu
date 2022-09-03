//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "blinn_phong_material.h"

namespace vox {
const Color& BlinnPhongMaterial::baseColor() { return _blinnPhongData.baseColor; }

void BlinnPhongMaterial::setBaseColor(const Color& newValue) {
    _blinnPhongData.baseColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

SampledTexture2DPtr BlinnPhongMaterial::baseTexture() { return _baseTexture; }

void BlinnPhongMaterial::setBaseTexture(SampledTexture2DPtr newValue) {
    _baseTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_baseTextureProp, BlinnPhongMaterial::_baseSamplerProp, newValue);

    if (newValue) {
        shaderData.enableMacro(HAS_DIFFUSE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_DIFFUSE_TEXTURE);
    }
}

const Color& BlinnPhongMaterial::specularColor() { return _blinnPhongData.specularColor; }

void BlinnPhongMaterial::setSpecularColor(const Color& newValue) {
    _blinnPhongData.specularColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

SampledTexture2DPtr BlinnPhongMaterial::specularTexture() { return _specularTexture; }

void BlinnPhongMaterial::setSpecularTexture(SampledTexture2DPtr newValue) {
    _specularTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_specularTextureProp, BlinnPhongMaterial::_specularSamplerProp,
                                 newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_SPECULAR_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_SPECULAR_TEXTURE);
    }
}

const Color& BlinnPhongMaterial::emissiveColor() { return _blinnPhongData.emissiveColor; }

void BlinnPhongMaterial::setEmissiveColor(const Color& newValue) {
    _blinnPhongData.emissiveColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

SampledTexture2DPtr BlinnPhongMaterial::emissiveTexture() { return _emissiveTexture; }

void BlinnPhongMaterial::BlinnPhongMaterial::setEmissiveTexture(SampledTexture2DPtr newValue) {
    _emissiveTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_emissiveTextureProp, BlinnPhongMaterial::_emissiveSamplerProp,
                                 newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_EMISSIVE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_EMISSIVE_TEXTURE);
    }
}

SampledTexture2DPtr BlinnPhongMaterial::normalTexture() { return _normalTexture; }

void BlinnPhongMaterial::setNormalTexture(SampledTexture2DPtr newValue) {
    _normalTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_normalTextureProp, BlinnPhongMaterial::_normalSamplerProp,
                                 newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_NORMAL_TEXTURE);
    }
}

float BlinnPhongMaterial::normalIntensity() { return _blinnPhongData.normalIntensity; }

void BlinnPhongMaterial::setNormalIntensity(float newValue) {
    _blinnPhongData.normalIntensity = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

float BlinnPhongMaterial::shininess() { return _blinnPhongData.shininess; }

void BlinnPhongMaterial::setShininess(float newValue) {
    _blinnPhongData.shininess = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

BlinnPhongMaterial::BlinnPhongMaterial(wgpu::Device& device)
    : BaseMaterial(device, Shader::find("blinn-phong")),
      _blinnPhongProp(Shader::createProperty("u_blinnPhongData", ShaderDataGroup::Material)),

      _baseTextureProp(Shader::createProperty("u_diffuseTexture", ShaderDataGroup::Material)),
      _baseSamplerProp(Shader::createProperty("u_diffuseSampler", ShaderDataGroup::Material)),

      _specularTextureProp(Shader::createProperty("u_specularTexture", ShaderDataGroup::Material)),
      _specularSamplerProp(Shader::createProperty("u_specularSampler", ShaderDataGroup::Material)),

      _emissiveTextureProp(Shader::createProperty("u_emissiveTexture", ShaderDataGroup::Material)),
      _emissiveSamplerProp(Shader::createProperty("u_emissiveSampler", ShaderDataGroup::Material)),

      _normalTextureProp(Shader::createProperty("u_normalTexture", ShaderDataGroup::Material)),
      _normalSamplerProp(Shader::createProperty("u_normalSampler", ShaderDataGroup::Material)) {
    shaderData.enableMacro(NEED_WORLDPOS);
    shaderData.setData(_blinnPhongProp, _blinnPhongData);
}

}  // namespace vox
