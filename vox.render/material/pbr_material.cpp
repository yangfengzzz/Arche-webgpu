//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_material.h"

namespace vox {
float PBRMaterial::metallic() {
    return _metallic;
}

void PBRMaterial::setMetallic(float newValue) {
    _metallic = newValue;
    shaderData.setData(PBRMaterial::_metallicProp, newValue);
}

float PBRMaterial::roughness() {
    return _roughness;
}

void PBRMaterial::setRoughness(float newValue) {
    _roughness = newValue;
    shaderData.setData(PBRMaterial::_roughnessProp, newValue);
}

SampledTexture2DPtr PBRMaterial::metallicRoughnessTexture() {
    return _metallicRoughnessTexture;
}

void PBRMaterial::setMetallicRoughnessTexture(SampledTexture2DPtr newValue) {
    _metallicRoughnessTexture = newValue;
    shaderData.setData(PBRMaterial::_metallicRoughnessTextureProp, newValue);
    if (newValue) {
        shaderData.enableMacro("HAS_METALROUGHNESSMAP");
    } else {
        shaderData.disableMacro("HAS_METALROUGHNESSMAP");
    }
}

PBRMaterial::PBRMaterial(wgpu::Device& device) :
PBRBaseMaterial(device),
_metallicProp(Shader::createProperty("u_metal", ShaderDataGroup::Material)),
_roughnessProp(Shader::createProperty("u_roughness", ShaderDataGroup::Material)),
_metallicRoughnessTextureProp(Shader::createProperty("u_metallicRoughnessTexture", ShaderDataGroup::Material)) {
    shaderData.enableMacro("IS_METALLIC_WORKFLOW");
    shaderData.setData(PBRMaterial::_metallicProp, _metallic);
    shaderData.setData(PBRMaterial::_roughnessProp, _roughness);
}

}
