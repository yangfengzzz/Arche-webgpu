//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unlit_material.h"

namespace vox {
const Color& UnlitMaterial::baseColor() { return _baseColor; }

void UnlitMaterial::setBaseColor(const Color& newValue) {
    _baseColor = newValue;
    shaderData.setData(UnlitMaterial::_baseColorProp, newValue);
}

SampledTexture2DPtr UnlitMaterial::baseTexture() { return _baseTexture; }

void UnlitMaterial::setBaseTexture(SampledTexture2DPtr newValue) {
    _baseTexture = newValue;
    shaderData.setSampledTexture(UnlitMaterial::_baseTextureProp, UnlitMaterial::_baseSamplerProp, newValue);

    if (newValue) {
        shaderData.enableMacro(HAS_BASE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_BASE_TEXTURE);
    }
}

UnlitMaterial::UnlitMaterial(wgpu::Device& device)
    : BaseMaterial(device, Shader::find("unlit")),
      _baseColorProp(Shader::createProperty("u_baseColor", ShaderDataGroup::Material)),
      _baseTextureProp(Shader::createProperty("u_baseTexture", ShaderDataGroup::Material)),
      _baseSamplerProp(Shader::createProperty("u_baseSampler", ShaderDataGroup::Material)) {
    shaderData.enableMacro(OMIT_NORMAL);

    shaderData.setData(_baseColorProp, _baseColor);
}

}  // namespace vox
