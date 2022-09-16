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

std::shared_ptr<Image> BlinnPhongMaterial::baseTexture() const { return _baseTexture; }

void BlinnPhongMaterial::setBaseTexture(const std::shared_ptr<Image>& newValue) {
    _baseTexture = newValue;
    shaderData.setImageView(BlinnPhongMaterial::_baseTextureProp, BlinnPhongMaterial::_baseSamplerProp,
                            newValue->getImageView());

    if (newValue) {
        shaderData.addDefine(HAS_DIFFUSE_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_DIFFUSE_TEXTURE);
    }
}

void BlinnPhongMaterial::setBaseSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(BlinnPhongMaterial::_baseSamplerProp, newValue);
}

const Color& BlinnPhongMaterial::specularColor() const { return _blinnPhongData.specularColor; }

void BlinnPhongMaterial::setSpecularColor(const Color& newValue) {
    _blinnPhongData.specularColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

std::shared_ptr<Image> BlinnPhongMaterial::specularTexture() const { return _specularTexture; }

void BlinnPhongMaterial::setSpecularTexture(const std::shared_ptr<Image>& newValue) {
    _specularTexture = newValue;
    shaderData.setImageView(BlinnPhongMaterial::_specularTextureProp, BlinnPhongMaterial::_specularSamplerProp,
                            newValue->getImageView());
    if (newValue) {
        shaderData.addDefine(HAS_SPECULAR_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_SPECULAR_TEXTURE);
    }
}

void BlinnPhongMaterial::setSpecularSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(BlinnPhongMaterial::_specularSamplerProp, newValue);
}

const Color& BlinnPhongMaterial::emissiveColor() const { return _blinnPhongData.emissiveColor; }

void BlinnPhongMaterial::setEmissiveColor(const Color& newValue) {
    _blinnPhongData.emissiveColor = newValue;
    shaderData.setData(BlinnPhongMaterial::_blinnPhongProp, _blinnPhongData);
}

std::shared_ptr<Image> BlinnPhongMaterial::emissiveTexture() const { return _emissiveTexture; }

void BlinnPhongMaterial::BlinnPhongMaterial::setEmissiveTexture(const std::shared_ptr<Image>& newValue) {
    _emissiveTexture = newValue;
    shaderData.setImageView(BlinnPhongMaterial::_emissiveTextureProp, BlinnPhongMaterial::_emissiveSamplerProp,
                            newValue->getImageView());
    if (newValue) {
        shaderData.addDefine(HAS_EMISSIVE_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_EMISSIVE_TEXTURE);
    }
}

void BlinnPhongMaterial::setEmissiveSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(BlinnPhongMaterial::_emissiveSamplerProp, newValue);
}

std::shared_ptr<Image> BlinnPhongMaterial::normalTexture() const { return _normalTexture; }

void BlinnPhongMaterial::setNormalTexture(const std::shared_ptr<Image>& newValue) {
    _normalTexture = newValue;
    shaderData.setImageView(BlinnPhongMaterial::_normalTextureProp, BlinnPhongMaterial::_normalSamplerProp,
                            newValue->getImageView());
    if (newValue) {
        shaderData.addDefine(HAS_NORMAL_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_NORMAL_TEXTURE);
    }
}

void BlinnPhongMaterial::setNormalSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(BlinnPhongMaterial::_normalSamplerProp, newValue);
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
