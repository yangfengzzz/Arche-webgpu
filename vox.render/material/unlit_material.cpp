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

SampledTexture2DPtr UnlitMaterial::baseTexture() const { return _baseTexture; }

void UnlitMaterial::setBaseTexture(const SampledTexture2DPtr& newValue) {
    _baseTexture = newValue;
    shaderData.setSampledTexture(UnlitMaterial::_baseTextureProp, UnlitMaterial::_baseSamplerProp, newValue);

    if (newValue) {
        shaderData.addDefine(HAS_BASE_TEXTURE);
    } else {
        shaderData.removeDefine(HAS_BASE_TEXTURE);
    }
}

UnlitMaterial::UnlitMaterial(wgpu::Device& device, const std::string& name) : BaseMaterial(device, name) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("unlit.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("unlit.frag");

    shaderData.addDefine(OMIT_NORMAL);

    shaderData.setData(_baseColorProp, _baseColor);
}

const std::string UnlitMaterial::_baseColorProp = "u_baseColor";
const std::string UnlitMaterial::_baseTextureProp = "u_baseTexture";
const std::string UnlitMaterial::_baseSamplerProp = "u_baseSampler";

}  // namespace vox
