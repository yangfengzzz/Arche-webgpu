//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_base_material.h"

namespace vox {
const Color& PBRBaseMaterial::baseColor() {
    return _baseColor;
}

void PBRBaseMaterial::setBaseColor(const Color &newValue) {
    _baseColor = newValue;
    shaderData.setData(PBRBaseMaterial::_baseColorProp, newValue);
}

SampledTexture2DPtr PBRBaseMaterial::baseTexture() {
    return _baseTexture;
}

void PBRBaseMaterial::setBaseTexture(SampledTexture2DPtr newValue) {
    _baseTexture = newValue;
    shaderData.setData(PBRBaseMaterial::_baseTextureProp, newValue);
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
    shaderData.setData(PBRBaseMaterial::_normalTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_NORMAL_TEXTURE);
    }
}

float PBRBaseMaterial::normalTextureIntensity() {
    return _normalTextureIntensity;
}

void PBRBaseMaterial::setNormalTextureIntensity(float newValue) {
    _normalTextureIntensity = newValue;
    shaderData.setData(PBRBaseMaterial::_normalTextureIntensityProp, newValue);
}

const Color& PBRBaseMaterial::emissiveColor() {
    return _emissiveColor;
}

void PBRBaseMaterial::setEmissiveColor(const Color &newValue) {
    _emissiveColor = newValue;
    shaderData.setData(PBRBaseMaterial::_emissiveColorProp, newValue);
}

SampledTexture2DPtr PBRBaseMaterial::emissiveTexture() {
    return _emissiveTexture;
}

void PBRBaseMaterial::setEmissiveTexture(SampledTexture2DPtr newValue) {
    _emissiveTexture = newValue;
    shaderData.setData(PBRBaseMaterial::_emissiveTextureProp, newValue);
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
    shaderData.setData(PBRBaseMaterial::_occlusionTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_OCCLUSIONMAP);
    } else {
        shaderData.disableMacro(HAS_OCCLUSIONMAP);
    }
}

float PBRBaseMaterial::occlusionTextureIntensity() {
    return _occlusionTextureIntensity;
}

void PBRBaseMaterial::setOcclusionTextureIntensity(float newValue) {
    _occlusionTextureIntensity = newValue;
    shaderData.setData(PBRBaseMaterial::_occlusionTextureIntensityProp, newValue);
}

const Vector4F& PBRBaseMaterial::tilingOffset() {
    return _tilingOffset;
}

void PBRBaseMaterial::setTilingOffset(const Vector4F &newValue) {
    _tilingOffset = newValue;
    shaderData.setData(PBRBaseMaterial::_tilingOffsetProp, newValue);
}

PBRBaseMaterial::PBRBaseMaterial(wgpu::Device& device) :
BaseMaterial(device, Shader::find("pbr")),
_tilingOffsetProp(Shader::createProperty("u_tilingOffset", ShaderDataGroup::Material)),
_normalTextureIntensityProp(Shader::createProperty("u_normalIntensity", ShaderDataGroup::Material)),
_occlusionTextureIntensityProp(Shader::createProperty("u_occlusionStrength", ShaderDataGroup::Material)),
_baseColorProp(Shader::createProperty("u_baseColor", ShaderDataGroup::Material)),
_emissiveColorProp(Shader::createProperty("u_emissiveColor", ShaderDataGroup::Material)),
_baseTextureProp(Shader::createProperty("u_baseColorTexture", ShaderDataGroup::Material)),
_normalTextureProp(Shader::createProperty("u_normalTexture", ShaderDataGroup::Material)),
_emissiveTextureProp(Shader::createProperty("u_emissiveTexture", ShaderDataGroup::Material)),
_occlusionTextureProp(Shader::createProperty("u_occlusionTexture", ShaderDataGroup::Material)) {
    shaderData.enableMacro(NEED_WORLDPOS);
    shaderData.enableMacro(NEED_TILINGOFFSET);
    
    shaderData.setData(PBRBaseMaterial::_baseColorProp, _baseColor);
    shaderData.setData(PBRBaseMaterial::_emissiveColorProp, _emissiveColor);
    shaderData.setData(PBRBaseMaterial::_tilingOffsetProp, _tilingOffset);
    
    shaderData.setData(PBRBaseMaterial::_normalTextureIntensityProp, _normalTextureIntensity);
    shaderData.setData(PBRBaseMaterial::_occlusionTextureIntensityProp, _occlusionTextureIntensity);
}

}
