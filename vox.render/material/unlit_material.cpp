//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/unlit_material.h"

#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
const Color& UnlitMaterial::baseColor() { return _baseColor; }

void UnlitMaterial::setBaseColor(const Color& newValue) {
    _baseColor = newValue;
    shaderData.setData(UnlitMaterial::_baseColorProp, newValue);
}

std::shared_ptr<Image> UnlitMaterial::baseTexture() const { return _baseTexture; }

void UnlitMaterial::setBaseTexture(const std::shared_ptr<Image>& newValue) {
    _baseTexture = newValue;
    if (newValue) {
        shaderData.setImageView(UnlitMaterial::_baseTextureProp, UnlitMaterial::_baseSamplerProp, newValue->getImageView());
        shaderData.addDefine(HAS_BASE_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_BASE_TEXTURE);
    }
}

void UnlitMaterial::setBaseSampler(const wgpu::SamplerDescriptor &desc) {
    shaderData.setSampler(UnlitMaterial::_baseSamplerProp, desc);
}

UnlitMaterial::UnlitMaterial(wgpu::Device& device, const std::string& name) : BaseMaterial(device, name) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("unlit.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("unlit.frag");

    shaderData.addDefine(OMIT_NORMAL);

    shaderData.setData(_baseColorProp, _baseColor);
}

const std::string UnlitMaterial::_baseColorProp = "u_baseColor";

}  // namespace vox
