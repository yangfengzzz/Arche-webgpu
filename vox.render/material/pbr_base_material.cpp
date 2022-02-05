//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_base_material.h"

namespace vox {
const Color& PBRBaseMaterial::baseColor() {
    return _pbrBaseData.baseColor;
}

void PBRBaseMaterial::setBaseColor(const Color &newValue) {
    _pbrBaseData.baseColor = newValue;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

SampledTexture2DPtr PBRBaseMaterial::baseTexture() {
    return _baseTexture;
}

void PBRBaseMaterial::setBaseTexture(SampledTexture2DPtr newValue) {
    _baseTexture = newValue;
    shaderData.setSampledTexture(PBRBaseMaterial::_baseTextureProp,
                                 PBRBaseMaterial::_baseSamplerProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_BASE_COLORMAP);
    } else {
        shaderData.disableMacro(HAS_BASE_COLORMAP);
    }
}

SampledTexture2DPtr PBRBaseMaterial::normalTexture() {
    return _normalTexture;
}

void PBRBaseMaterial::setNormalTexture(SampledTexture2DPtr newValue) {
    _normalTexture = newValue;
    shaderData.setSampledTexture(PBRBaseMaterial::_normalTextureProp,
                                 PBRBaseMaterial::_normalSamplerProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_NORMAL_TEXTURE);
    }
}

float PBRBaseMaterial::normalTextureIntensity() {
    return _pbrBaseData.normalTextureIntensity;
}

void PBRBaseMaterial::setNormalTextureIntensity(float newValue) {
    _pbrBaseData.normalTextureIntensity = newValue;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

const Color& PBRBaseMaterial::emissiveColor() {
    return _pbrBaseData.emissiveColor;
}

void PBRBaseMaterial::setEmissiveColor(const Color &newValue) {
    _pbrBaseData.emissiveColor = newValue;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

SampledTexture2DPtr PBRBaseMaterial::emissiveTexture() {
    return _emissiveTexture;
}

void PBRBaseMaterial::setEmissiveTexture(SampledTexture2DPtr newValue) {
    _emissiveTexture = newValue;
    shaderData.setSampledTexture(PBRBaseMaterial::_emissiveTextureProp,
                                 PBRBaseMaterial::_emissiveSamplerProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_EMISSIVEMAP);
    } else {
        shaderData.disableMacro(HAS_EMISSIVEMAP);
    }
}

SampledTexture2DPtr PBRBaseMaterial::occlusionTexture() {
    return _occlusionTexture;
}

void PBRBaseMaterial::setOcclusionTexture(SampledTexture2DPtr newValue) {
    _occlusionTexture = newValue;
    shaderData.setSampledTexture(PBRBaseMaterial::_occlusionTextureProp,
                                 PBRBaseMaterial::_occlusionSamplerProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_OCCLUSIONMAP);
    } else {
        shaderData.disableMacro(HAS_OCCLUSIONMAP);
    }
}

float PBRBaseMaterial::occlusionTextureIntensity() {
    return _pbrBaseData.occlusionTextureIntensity;
}

void PBRBaseMaterial::setOcclusionTextureIntensity(float newValue) {
    _pbrBaseData.occlusionTextureIntensity = newValue;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

PBRBaseMaterial::PBRBaseMaterial(wgpu::Device& device, Shader *shader) :
BaseMaterial(device, shader),
_pbrBaseProp(Shader::createProperty("u_pbrBaseData", ShaderDataGroup::Material)),

_baseTextureProp(Shader::createProperty("u_baseColorTexture", ShaderDataGroup::Material)),
_baseSamplerProp(Shader::createProperty("u_baseColorSampler", ShaderDataGroup::Material)),

_normalTextureProp(Shader::createProperty("u_normalTexture", ShaderDataGroup::Material)),
_normalSamplerProp(Shader::createProperty("u_normalSampler", ShaderDataGroup::Material)),

_emissiveTextureProp(Shader::createProperty("u_emissiveTexture", ShaderDataGroup::Material)),
_emissiveSamplerProp(Shader::createProperty("u_emissiveSampler", ShaderDataGroup::Material)),

_occlusionTextureProp(Shader::createProperty("u_occlusionTexture", ShaderDataGroup::Material)),
_occlusionSamplerProp(Shader::createProperty("u_occlusionSampler", ShaderDataGroup::Material)) {
    shaderData.enableMacro(NEED_WORLDPOS);
    shaderData.enableMacro(NEED_TILINGOFFSET);
    
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

}
