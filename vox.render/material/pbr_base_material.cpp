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

std::shared_ptr<Image> PBRBaseMaterial::baseTexture() const { return _baseTexture; }

void PBRBaseMaterial::setBaseTexture(const std::shared_ptr<Image>& newValue) {
    _baseTexture = newValue;
    if (newValue) {
        shaderData.setImageView(PBRBaseMaterial::_baseTextureProp, PBRBaseMaterial::_baseSamplerProp,
                                newValue->getImageView());
        shaderData.addDefine(HAS_BASE_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_BASE_TEXTURE);
    }
}

void PBRBaseMaterial::setBaseSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(PBRBaseMaterial::_baseSamplerProp, newValue);
}

std::shared_ptr<Image> PBRBaseMaterial::normalTexture() const { return _normalTexture; }

void PBRBaseMaterial::setNormalTexture(const std::shared_ptr<Image>& newValue) {
    _normalTexture = newValue;
    if (newValue) {
        shaderData.setImageView(PBRBaseMaterial::_normalTextureProp, PBRBaseMaterial::_normalSamplerProp,
                                newValue->getImageView());
        shaderData.addDefine(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_NORMAL_TEXTURE);
    }
}

void PBRBaseMaterial::setNormalSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(PBRBaseMaterial::_normalSamplerProp, newValue);
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

std::shared_ptr<Image> PBRBaseMaterial::emissiveTexture() const { return _emissiveTexture; }

void PBRBaseMaterial::setEmissiveTexture(const std::shared_ptr<Image>& newValue) {
    _emissiveTexture = newValue;
    if (newValue) {
        shaderData.setImageView(PBRBaseMaterial::_emissiveTextureProp, PBRBaseMaterial::_emissiveSamplerProp,
                                newValue->getImageView());
        shaderData.addDefine(HAS_EMISSIVE_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_EMISSIVE_TEXTURE);
    }
}

void PBRBaseMaterial::setEmissiveSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(PBRBaseMaterial::_emissiveSamplerProp, newValue);
}

std::shared_ptr<Image> PBRBaseMaterial::occlusionTexture() const { return _occlusionTexture; }

void PBRBaseMaterial::setOcclusionTexture(const std::shared_ptr<Image>& newValue) {
    _occlusionTexture = newValue;
    if (newValue) {
        shaderData.setImageView(PBRBaseMaterial::_occlusionTextureProp, PBRBaseMaterial::_occlusionSamplerProp,
                                newValue->getImageView());
        shaderData.addDefine(HAS_OCCLUSION_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_OCCLUSION_TEXTURE);
    }
}

void PBRBaseMaterial::setOcclusionSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(PBRBaseMaterial::_occlusionSamplerProp, newValue);
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
        shaderData.removeDefine(HAS_CLEARCOAT);
    } else {
        shaderData.addDefine(HAS_CLEARCOAT);
    }
    _pbrBaseData.clearCoat = value;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

std::shared_ptr<Image> PBRBaseMaterial::clearCoatTexture() const { return _clearCoatTexture; }

void PBRBaseMaterial::setClearCoatTexture(const std::shared_ptr<Image>& value) {
    _clearCoatTexture = value;
    if (value) {
        shaderData.setImageView(PBRBaseMaterial::_clearCoatTextureProp, PBRBaseMaterial::_clearCoatSamplerProp,
                                value->getImageView());
        shaderData.addDefine(HAS_CLEARCOAT_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_CLEARCOAT_TEXTURE);
    }
}

void PBRBaseMaterial::setClearCoatSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(PBRBaseMaterial::_clearCoatSamplerProp, newValue);
}

float PBRBaseMaterial::clearCoatRoughness() const { return _pbrBaseData.clearCoatRoughness; }

void PBRBaseMaterial::setClearCoatRoughness(float value) {
    _pbrBaseData.clearCoatRoughness = value;
    shaderData.setData(PBRBaseMaterial::_pbrBaseProp, _pbrBaseData);
}

std::shared_ptr<Image> PBRBaseMaterial::clearCoatRoughnessTexture() const { return _clearCoatRoughnessTexture; }

void PBRBaseMaterial::setClearCoatRoughnessTexture(const std::shared_ptr<Image>& value) {
    _clearCoatRoughnessTexture = value;
    if (value) {
        shaderData.setImageView(PBRBaseMaterial::_clearCoatRoughnessTextureProp,
                                PBRBaseMaterial::_clearCoatRoughnessSamplerProp, value->getImageView());
        shaderData.addDefine(HAS_CLEARCOATROUGHNESS_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_CLEARCOATROUGHNESS_TEXTURE);
    }
}

void PBRBaseMaterial::setClearCoatRoughnessSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(PBRBaseMaterial::_clearCoatRoughnessSamplerProp, newValue);
}

std::shared_ptr<Image> PBRBaseMaterial::clearCoatNormalTexture() const { return _clearCoatNormalTexture; }

void PBRBaseMaterial::setClearCoatNormalTexture(const std::shared_ptr<Image>& value) {
    _clearCoatNormalTexture = value;
    if (value) {
        shaderData.setImageView(PBRBaseMaterial::_clearCoatNormalTextureProp, PBRBaseMaterial::_clearCoatNormalSamplerProp,
                                value->getImageView());
        shaderData.addDefine(HAS_CLEARCOATNORMAL_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_CLEARCOATNORMAL_TEXTURE);
    }
}

void PBRBaseMaterial::setClearCoatNormalSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(PBRBaseMaterial::_clearCoatNormalSamplerProp, newValue);
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
