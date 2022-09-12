//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/blinn_phong_material.h"

#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
const Color& BlinnPhongMaterial::baseColor() const { return _blinnPhongData.baseColor; }

void BlinnPhongMaterial::setBaseColor(const Color& newValue) {
    _blinnPhongData.baseColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

SampledTexture2DPtr BlinnPhongMaterial::baseTexture() const { return _baseTexture; }

void BlinnPhongMaterial::setBaseTexture(const SampledTexture2DPtr& newValue) {
    _baseTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_baseTextureProp, BlinnPhongMaterial::_baseSamplerProp, newValue);

    if (newValue) {
        shaderData.addDefine(HAS_DIFFUSE_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_DIFFUSE_TEXTURE);
    }
}

const Color& BlinnPhongMaterial::specularColor() const { return _blinnPhongData.specularColor; }

void BlinnPhongMaterial::setSpecularColor(const Color& newValue) {
    _blinnPhongData.specularColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

SampledTexture2DPtr BlinnPhongMaterial::specularTexture() const { return _specularTexture; }

void BlinnPhongMaterial::setSpecularTexture(const SampledTexture2DPtr& newValue) {
    _specularTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_specularTextureProp, BlinnPhongMaterial::_specularSamplerProp,
                                 newValue);
    if (newValue) {
        shaderData.addDefine(HAS_SPECULAR_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_SPECULAR_TEXTURE);
    }
}

const Color& BlinnPhongMaterial::emissiveColor() const { return _blinnPhongData.emissiveColor; }

void BlinnPhongMaterial::setEmissiveColor(const Color& newValue) {
    _blinnPhongData.emissiveColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

SampledTexture2DPtr BlinnPhongMaterial::emissiveTexture() const { return _emissiveTexture; }

void BlinnPhongMaterial::BlinnPhongMaterial::setEmissiveTexture(const SampledTexture2DPtr& newValue) {
    _emissiveTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_emissiveTextureProp, BlinnPhongMaterial::_emissiveSamplerProp,
                                 newValue);
    if (newValue) {
        shaderData.addDefine(HAS_EMISSIVE_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_EMISSIVE_TEXTURE);
    }
}

SampledTexture2DPtr BlinnPhongMaterial::normalTexture() const { return _normalTexture; }

void BlinnPhongMaterial::setNormalTexture(const SampledTexture2DPtr& newValue) {
    _normalTexture = newValue;
    shaderData.setSampledTexture(BlinnPhongMaterial::_normalTextureProp, BlinnPhongMaterial::_normalSamplerProp,
                                 newValue);
    if (newValue) {
        shaderData.addDefine(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_NORMAL_TEXTURE);
    }
}

float BlinnPhongMaterial::normalIntensity() const { return _blinnPhongData.normalIntensity; }

void BlinnPhongMaterial::setNormalIntensity(float newValue) {
    _blinnPhongData.normalIntensity = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

float BlinnPhongMaterial::shininess() const { return _blinnPhongData.shininess; }

void BlinnPhongMaterial::setShininess(float newValue) {
    _blinnPhongData.shininess = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

BlinnPhongMaterial::BlinnPhongMaterial(wgpu::Device& device, const std::string& name) : BaseMaterial(device, name) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("blinn-phong.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("blinn-phong.frag");

    shaderData.addDefine(NEED_WORLDPOS);
    shaderData.setData(_blinnPhongProp, _blinnPhongData);
}

const std::string BlinnPhongMaterial::_blinnPhongProp = "u_blinnPhongData";
const std::string BlinnPhongMaterial::_specularTextureProp = "u_specularTexture";
const std::string BlinnPhongMaterial::_specularSamplerProp = "u_specularSampler";

}  // namespace vox
