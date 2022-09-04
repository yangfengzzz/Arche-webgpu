//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/pbr_material.h"

#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
float PBRMaterial::metallic() const { return _pbrData.metallic; }

void PBRMaterial::setMetallic(float newValue) {
    _pbrData.metallic = newValue;
    shaderData.setData(PBRMaterial::_pbrProp, _pbrData);
}

float PBRMaterial::roughness() const { return _pbrData.roughness; }

void PBRMaterial::setRoughness(float newValue) {
    _pbrData.roughness = newValue;
    shaderData.setData(PBRMaterial::_pbrProp, _pbrData);
}

SampledTexture2DPtr PBRMaterial::metallicRoughnessTexture() { return _metallicRoughnessTexture; }

void PBRMaterial::setMetallicRoughnessTexture(const SampledTexture2DPtr &newValue) {
    _metallicRoughnessTexture = newValue;
    shaderData.setSampledTexture(PBRMaterial::_metallicRoughnessTextureProp, PBRMaterial::_metallicRoughnessSamplerProp,
                                 newValue);
    if (newValue) {
        shaderData.addDefine(HAS_METALROUGHNESSMAP);
    } else {
        shaderData.removeDefine(HAS_METALROUGHNESSMAP);
    }
}

PBRMaterial::PBRMaterial(wgpu::Device &device, const std::string &name)
    : PBRBaseMaterial(device, name),
      _pbrProp("u_pbrData"),
      _metallicRoughnessTextureProp("u_metallicRoughnessTexture"),
      _metallicRoughnessSamplerProp("u_metallicRoughnessSampler") {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/blinn-phong.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/pbr.frag");

    shaderData.addDefine("IS_METALLIC_WORKFLOW");
    shaderData.setData(PBRMaterial::_pbrProp, _pbrData);
}

}  // namespace vox
