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

std::shared_ptr<Image> PBRMaterial::metallicRoughnessTexture() const { return _metallicRoughnessTexture; }

void PBRMaterial::setMetallicRoughnessTexture(const std::shared_ptr<Image> &newValue) {
    _metallicRoughnessTexture = newValue;
    shaderData.setImageView(PBRMaterial::_metallicRoughnessTextureProp, PBRMaterial::_metallicRoughnessSamplerProp,
                            newValue->getImageView());
    if (newValue) {
        shaderData.addDefine(HAS_METALROUGHNESSMAP);
    } else {
        shaderData.removeDefine(HAS_METALROUGHNESSMAP);
    }
}

void PBRMaterial::setMetallicRoughnessSampler(const wgpu::SamplerDescriptor &newValue) {
    shaderData.setSampler(PBRMaterial::_metallicRoughnessSamplerProp, newValue);
}

PBRMaterial::PBRMaterial(wgpu::Device &device, const std::string &name) : PBRBaseMaterial(device, name) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("blinn-phong.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("pbr.frag");

    shaderData.addDefine("IS_METALLIC_WORKFLOW");
    shaderData.setData(PBRMaterial::_pbrProp, _pbrData);
}

const std::string PBRMaterial::_pbrProp = "u_pbrData";
const std::string PBRMaterial::_metallicRoughnessTextureProp = "u_metallicRoughnessTexture";
const std::string PBRMaterial::_metallicRoughnessSamplerProp = "u_metallicRoughnessSampler";

}  // namespace vox
