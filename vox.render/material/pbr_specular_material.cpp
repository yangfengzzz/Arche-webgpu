//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#include "pbr_specular_material.h"

namespace vox {
const Color& PBRSpecularMaterial::specularColor() {
    return _specularColor;
}

void PBRSpecularMaterial::setSpecularColor(const Color &newValue) {
    _specularColor = newValue;
    shaderData.setData(PBRSpecularMaterial::_specularColorProp, newValue);
}

float PBRSpecularMaterial::glossiness() {
    return _glossiness;
}

void PBRSpecularMaterial::setGlossiness(float newValue) {
    _glossiness = newValue;
    shaderData.setData(PBRSpecularMaterial::_glossinessProp, newValue);
}

SampledTexture2DPtr PBRSpecularMaterial::specularGlossinessTexture() {
    return _specularGlossinessTexture;
}

void PBRSpecularMaterial::setSpecularGlossinessTexture(SampledTexture2DPtr newValue) {
    _specularGlossinessTexture = newValue;
    shaderData.setSampledTexture(PBRSpecularMaterial::_specularGlossinessTextureProp,
                                 PBRSpecularMaterial::_specularGlossinessSamplerProp, newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_SPECULARGLOSSINESSMAP);
    } else {
        shaderData.disableMacro(HAS_SPECULARGLOSSINESSMAP);
    }
}

PBRSpecularMaterial::PBRSpecularMaterial(wgpu::Device& device) :
PBRBaseMaterial(device),
_glossinessProp(Shader::createProperty("u_glossiness", ShaderDataGroup::Material)),
_specularColorProp(Shader::createProperty("u_specularColor", ShaderDataGroup::Material)),
_specularGlossinessTextureProp(Shader::createProperty("_specularGlossinessTexture", ShaderDataGroup::Material)),
_specularGlossinessSamplerProp(Shader::createProperty("_specularGlossinessSampler", ShaderDataGroup::Material)) {
    shaderData.setData(PBRSpecularMaterial::_specularColorProp, _specularColor);
    shaderData.setData(PBRSpecularMaterial::_glossinessProp, _glossiness);
}

}
