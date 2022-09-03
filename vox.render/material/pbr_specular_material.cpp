//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/pbr_specular_material.h"

namespace vox {
const Color& PBRSpecularMaterial::specularColor() const { return _pbrSpecularData.specularColor; }

void PBRSpecularMaterial::setSpecularColor(const Color& newValue) {
    _pbrSpecularData.specularColor = newValue;
    shaderData.setData(PBRSpecularMaterial::_pbrSpecularProp, _pbrSpecularData);
}

float PBRSpecularMaterial::glossiness() const { return _pbrSpecularData.glossiness; }

void PBRSpecularMaterial::setGlossiness(float newValue) {
    _pbrSpecularData.glossiness = newValue;
    shaderData.setData(PBRSpecularMaterial::_pbrSpecularProp, _pbrSpecularData);
}

SampledTexture2DPtr PBRSpecularMaterial::specularGlossinessTexture() { return _specularGlossinessTexture; }

void PBRSpecularMaterial::setSpecularGlossinessTexture(const SampledTexture2DPtr& newValue) {
    _specularGlossinessTexture = newValue;
    shaderData.setSampledTexture(PBRSpecularMaterial::_specularGlossinessTextureProp,
                                 PBRSpecularMaterial::_specularGlossinessSamplerProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_SPECULARGLOSSINESSMAP);
    } else {
        shaderData.disableMacro(HAS_SPECULARGLOSSINESSMAP);
    }
}

PBRSpecularMaterial::PBRSpecularMaterial(wgpu::Device& device)
    : PBRBaseMaterial(device, Shader::find("pbr-specular")),
      _pbrSpecularProp(Shader::createProperty("u_pbrSpecularData", ShaderDataGroup::Material)),
      _specularGlossinessTextureProp(Shader::createProperty("_specularGlossinessTexture", ShaderDataGroup::Material)),
      _specularGlossinessSamplerProp(Shader::createProperty("_specularGlossinessSampler", ShaderDataGroup::Material)) {
    shaderData.setData(PBRSpecularMaterial::_pbrSpecularProp, _pbrSpecularData);
}

}  // namespace vox
