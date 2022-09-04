//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/pbr_base_material.h"

#include "vox.render/shader/internal_variant_name.h"

namespace vox {
const Color& PBRBaseMaterial::baseColor() const { return _pbrBaseData.baseColor; }

void PBRBaseMaterial::setBaseColor(const Color& newValue) {
    _pbrBaseData.baseColor = newValue;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

SampledTexture2DPtr PBRBaseMaterial::baseTexture() { return _baseTexture; }

void PBRBaseMaterial::setBaseTexture(const SampledTexture2DPtr& newValue) {
    _baseTexture = newValue;
    shaderData.setSampledTexture(PBRBaseMaterial::_baseTextureProp, PBRBaseMaterial::_baseSamplerProp, newValue);
    if (newValue) {
        shaderData.addDefine(HAS_BASE_COLORMAP);
    } else {
        shaderData.removeDefine(HAS_BASE_COLORMAP);
    }
}

SampledTexture2DPtr PBRBaseMaterial::normalTexture() { return _normalTexture; }

void PBRBaseMaterial::setNormalTexture(const SampledTexture2DPtr& newValue) {
    _normalTexture = newValue;
    shaderData.setSampledTexture(PBRBaseMaterial::_normalTextureProp, PBRBaseMaterial::_normalSamplerProp, newValue);
    if (newValue) {
        shaderData.addDefine(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_NORMAL_TEXTURE);
    }
}

float PBRBaseMaterial::normalTextureIntensity() const { return _pbrBaseData.normalTextureIntensity; }

void PBRBaseMaterial::setNormalTextureIntensity(float newValue) {
    _pbrBaseData.normalTextureIntensity = newValue;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

const Color& PBRBaseMaterial::emissiveColor() const { return _pbrBaseData.emissiveColor; }

void PBRBaseMaterial::setEmissiveColor(const Color& newValue) {
    _pbrBaseData.emissiveColor = newValue;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

SampledTexture2DPtr PBRBaseMaterial::emissiveTexture() { return _emissiveTexture; }

void PBRBaseMaterial::setEmissiveTexture(const SampledTexture2DPtr& newValue) {
    _emissiveTexture = newValue;
    shaderData.setSampledTexture(PBRBaseMaterial::_emissiveTextureProp, PBRBaseMaterial::_emissiveSamplerProp,
                                 newValue);
    if (newValue) {
        shaderData.addDefine(HAS_EMISSIVEMAP);
    } else {
        shaderData.removeDefine(HAS_EMISSIVEMAP);
    }
}

SampledTexture2DPtr PBRBaseMaterial::occlusionTexture() { return _occlusionTexture; }

void PBRBaseMaterial::setOcclusionTexture(const SampledTexture2DPtr& newValue) {
    _occlusionTexture = newValue;
    shaderData.setSampledTexture(PBRBaseMaterial::_occlusionTextureProp, PBRBaseMaterial::_occlusionSamplerProp,
                                 newValue);
    if (newValue) {
        shaderData.addDefine(HAS_OCCLUSIONMAP);
    } else {
        shaderData.removeDefine(HAS_OCCLUSIONMAP);
    }
}

float PBRBaseMaterial::occlusionTextureIntensity() const { return _pbrBaseData.occlusionTextureIntensity; }

void PBRBaseMaterial::setOcclusionTextureIntensity(float newValue) {
    _pbrBaseData.occlusionTextureIntensity = newValue;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

PBRBaseMaterial::PBRBaseMaterial(wgpu::Device& device, const std::string& name)
    : BaseMaterial(device, name),
      _pbrBaseProp("u_pbrBaseData"),

      _baseTextureProp("u_baseColorTexture"),
      _baseSamplerProp("u_baseColorSampler"),

      _normalTextureProp("u_normalTexture"),
      _normalSamplerProp("u_normalSampler"),

      _emissiveTextureProp("u_emissiveTexture"),
      _emissiveSamplerProp("u_emissiveSampler"),

      _occlusionTextureProp("u_occlusionTexture"),
      _occlusionSamplerProp("u_occlusionSampler") {
    shaderData.addDefine(NEED_WORLDPOS);
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

}  // namespace vox
