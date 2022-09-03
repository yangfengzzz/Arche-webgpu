//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pbr_material.h"

namespace vox {
float PBRMaterial::metallic() { return _pbrData.metallic; }

void PBRMaterial::setMetallic(float newValue) {
    _pbrData.metallic = newValue;
    shaderData.setData(PBRMaterial::_pbrProp, _pbrData);
}

float PBRMaterial::roughness() { return _pbrData.roughness; }

void PBRMaterial::setRoughness(float newValue) {
    _pbrData.roughness = newValue;
    shaderData.setData(PBRMaterial::_pbrProp, _pbrData);
}

SampledTexture2DPtr PBRMaterial::metallicRoughnessTexture() { return _metallicRoughnessTexture; }

void PBRMaterial::setMetallicRoughnessTexture(SampledTexture2DPtr newValue) {
    _metallicRoughnessTexture = newValue;
    shaderData.setSampledTexture(PBRMaterial::_metallicRoughnessTextureProp, PBRMaterial::_metallicRoughnessSamplerProp,
                                 newValue);
    if (newValue) {
        shaderData.enableMacro(HAS_METALROUGHNESSMAP);
    } else {
        shaderData.disableMacro(HAS_METALROUGHNESSMAP);
    }
}

PBRMaterial::PBRMaterial(wgpu::Device& device)
    : PBRBaseMaterial(device, Shader::find("pbr")),
      _pbrProp(Shader::createProperty("u_pbrData", ShaderDataGroup::Material)),
      _metallicRoughnessTextureProp(Shader::createProperty("u_metallicRoughnessTexture", ShaderDataGroup::Material)),
      _metallicRoughnessSamplerProp(Shader::createProperty("u_metallicRoughnessSampler", ShaderDataGroup::Material)) {
    shaderData.setData(PBRMaterial::_pbrProp, _pbrData);
}

}  // namespace vox
