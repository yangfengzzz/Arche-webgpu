//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/pbr_base_material.h"

#include "vox.base/logging.h"
#include "vox.render/shader/internal_variant_name.h"

namespace vox {
const Color& PBRBaseMaterial::baseColor() const { return _pbrBaseData.baseColor; }

void PBRBaseMaterial::setBaseColor(const Color& newValue) {
    _pbrBaseData.baseColor = newValue;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

SampledTexture2DPtr PBRBaseMaterial::baseTexture() const { return _baseTexture; }

void PBRBaseMaterial::setBaseTexture(const SampledTexture2DPtr& newValue) {
    _baseTexture = newValue;
    shaderData.setSampledTexture(PBRBaseMaterial::_baseTextureProp, PBRBaseMaterial::_baseSamplerProp, newValue);
    if (newValue) {
        shaderData.addDefine(HAS_BASE_COLORMAP);
    } else {
        shaderData.removeDefine(HAS_BASE_COLORMAP);
    }
}

SampledTexture2DPtr PBRBaseMaterial::normalTexture() const { return _normalTexture; }

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

Color PBRBaseMaterial::emissiveColor() const {
    return {_pbrBaseData.emissiveColor.x, _pbrBaseData.emissiveColor.y, _pbrBaseData.emissiveColor.z, 1.0};
}

void PBRBaseMaterial::setEmissiveColor(const Color& newValue) {
    _pbrBaseData.emissiveColor.set(newValue.r, newValue.g, newValue.b);
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

SampledTexture2DPtr PBRBaseMaterial::emissiveTexture() const { return _emissiveTexture; }

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

SampledTexture2DPtr PBRBaseMaterial::occlusionTexture() const { return _occlusionTexture; }

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

TextureCoordinate PBRBaseMaterial::occlusionTextureCoord() const { return _pbrBaseData.occlusionTextureCoord; }

void PBRBaseMaterial::setOcclusionTextureCoord(TextureCoordinate value) {
    if (value > TextureCoordinate::UV1) {
        LOGW("Occlusion texture uv coordinate must be UV0 or UV1.")
    }
    _pbrBaseData.occlusionTextureCoord = value;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

float PBRBaseMaterial::clearCoat() const { return _pbrBaseData.clearCoat; }

void PBRBaseMaterial::setClearCoat(float value) {
    if (value == 0) {
        shaderData.removeDefine("CLEARCOAT");
    } else {
        shaderData.addDefine("CLEARCOAT");
    }
    _pbrBaseData.clearCoat = value;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

SampledTexture2DPtr PBRBaseMaterial::clearCoatTexture() const { return _clearCoatTexture; }

void PBRBaseMaterial::setClearCoatTexture(const SampledTexture2DPtr& value) {
    if (value) {
        shaderData.addDefine("HAS_CLEARCOATTEXTURE");
    } else {
        shaderData.removeDefine("HAS_CLEARCOATTEXTURE");
    }
    _clearCoatTexture = value;
    shaderData.setSampledTexture(PBRBaseMaterial::_clearCoatTextureProp, PBRBaseMaterial::_clearCoatSamplerProp, value);
}

float PBRBaseMaterial::clearCoatRoughness() const { return _pbrBaseData.clearCoatRoughness; }

void PBRBaseMaterial::setClearCoatRoughness(float value) {
    _pbrBaseData.clearCoatRoughness = value;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

SampledTexture2DPtr PBRBaseMaterial::clearCoatRoughnessTexture() const { return _clearCoatRoughnessTexture; }

void PBRBaseMaterial::setClearCoatRoughnessTexture(const SampledTexture2DPtr& value) {
    if (value) {
        shaderData.addDefine("HAS_CLEARCOATROUGHNESSTEXTURE");
    } else {
        shaderData.removeDefine("HAS_CLEARCOATROUGHNESSTEXTURE");
    }
    _clearCoatRoughnessTexture = value;
    shaderData.setSampledTexture(PBRBaseMaterial::_clearCoatRoughnessTextureProp,
                                 PBRBaseMaterial::_clearCoatRoughnessSamplerProp, value);
}

SampledTexture2DPtr PBRBaseMaterial::clearCoatNormalTexture() const { return _clearCoatNormalTexture; }

void PBRBaseMaterial::setClearCoatNormalTexture(const SampledTexture2DPtr& value) {
    if (value) {
        shaderData.addDefine("HAS_CLEARCOATNORMALTEXTURE");
    } else {
        shaderData.removeDefine("HAS_CLEARCOATNORMALTEXTURE");
    }
    _clearCoatNormalTexture = value;
    shaderData.setSampledTexture(PBRBaseMaterial::_clearCoatNormalTextureProp,
                                 PBRBaseMaterial::_clearCoatNormalSamplerProp, value);
}

PBRBaseMaterial::PBRBaseMaterial(wgpu::Device& device, const std::string& name) : BaseMaterial(device, name) {
    shaderData.addDefine(NEED_WORLDPOS);
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

const std::string PBRBaseMaterial::_pbrBaseProp = "u_pbrBaseData";

const std::string PBRBaseMaterial::_occlusionTextureProp = "u_occlusionTexture";
const std::string PBRBaseMaterial::_occlusionSamplerProp = "u_occlusionSampler";
const std::string PBRBaseMaterial::_clearCoatTextureProp = "u_clearCoatTexture";
const std::string PBRBaseMaterial::_clearCoatSamplerProp = "u_clearCoatSampler";
const std::string PBRBaseMaterial::_clearCoatRoughnessTextureProp = "u_clearCoatRoughnessTexture";
const std::string PBRBaseMaterial::_clearCoatRoughnessSamplerProp = "u_clearCoatRoughnessSampler";
const std::string PBRBaseMaterial::_clearCoatNormalTextureProp = "u_clearCoatNormalTexture";
const std::string PBRBaseMaterial::_clearCoatNormalSamplerProp = "u_clearCoatNormalSampler";
}  // namespace vox
