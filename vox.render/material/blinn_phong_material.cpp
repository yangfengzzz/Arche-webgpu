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

SampledTexture2DPtr BlinnPhongMaterial::baseTexture() { return _baseTexture; }

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

SampledTexture2DPtr BlinnPhongMaterial::specularTexture() { return _specularTexture; }

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

SampledTexture2DPtr BlinnPhongMaterial::emissiveTexture() { return _emissiveTexture; }

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

SampledTexture2DPtr BlinnPhongMaterial::normalTexture() { return _normalTexture; }

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

BlinnPhongMaterial::BlinnPhongMaterial(wgpu::Device& device, const std::string& name)
    : BaseMaterial(device, name),
      _blinnPhongProp("u_blinnPhongData"),

      _baseTextureProp("u_diffuseTexture"),
      _baseSamplerProp("u_diffuseSampler"),

      _specularTextureProp("u_specularTexture"),
      _specularSamplerProp("u_specularSampler"),

      _emissiveTextureProp("u_emissiveTexture"),
      _emissiveSamplerProp("u_emissiveSampler"),

      _normalTextureProp("u_normalTexture"),
      _normalSamplerProp("u_normalSampler") {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("blinn-phong.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("blinn-phong.frag");

    shaderData.addDefine(NEED_WORLDPOS);
    shaderData.setData(_blinnPhongProp, _blinnPhongData);
}

}  // namespace vox
