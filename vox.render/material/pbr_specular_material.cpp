//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/pbr_specular_material.h"

#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
Color PBRSpecularMaterial::specularColor() const {
    return {_pbrSpecularData.specularColor.x, _pbrSpecularData.specularColor.y, _pbrSpecularData.specularColor.z, 1.0};
}

void PBRSpecularMaterial::setSpecularColor(const Color& newValue) {
    _pbrSpecularData.specularColor.set(newValue.r, newValue.g, newValue.b);
    shaderData.setData(PBRSpecularMaterial::_pbrSpecularProp, _pbrSpecularData);
}

float PBRSpecularMaterial::glossiness() const { return _pbrSpecularData.glossiness; }

void PBRSpecularMaterial::setGlossiness(float newValue) {
    _pbrSpecularData.glossiness = newValue;
    shaderData.setData(PBRSpecularMaterial::_pbrSpecularProp, _pbrSpecularData);
}

std::shared_ptr<Image> PBRSpecularMaterial::specularGlossinessTexture() const { return _specularGlossinessTexture; }

void PBRSpecularMaterial::setSpecularGlossinessTexture(const std::shared_ptr<Image>& newValue) {
    _specularGlossinessTexture = newValue;
    shaderData.setImageView(PBRSpecularMaterial::_specularGlossinessTextureProp,
                            PBRSpecularMaterial::_specularGlossinessSamplerProp, newValue->getImageView());
    if (newValue) {
        shaderData.addDefine(HAS_SPECULARGLOSSINESSMAP);
    } else {
        shaderData.removeDefine(HAS_SPECULARGLOSSINESSMAP);
    }
}

void PBRSpecularMaterial::setSpecularGlossinessSampler(const wgpu::SamplerDescriptor& newValue) {
    shaderData.setSampler(PBRSpecularMaterial::_specularGlossinessSamplerProp, newValue);
}

PBRSpecularMaterial::PBRSpecularMaterial(wgpu::Device& device, const std::string& name)
    : PBRBaseMaterial(device, name) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("blinn-phong.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("pbr.frag");
    shaderData.setData(PBRSpecularMaterial::_pbrSpecularProp, _pbrSpecularData);
}

const std::string PBRSpecularMaterial::_pbrSpecularProp = "u_pbrSpecularData";
const std::string PBRSpecularMaterial::_specularGlossinessTextureProp = "u_specularGlossinessTexture";
const std::string PBRSpecularMaterial::_specularGlossinessSamplerProp = "u_specularGlossinessSampler";

}  // namespace vox
