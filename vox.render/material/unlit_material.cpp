//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unlit_material.h"

namespace vox {
const Color& UnlitMaterial::baseColor() {
    return _baseColor;
}

void UnlitMaterial::setBaseColor(const Color &newValue) {
    _baseColor = newValue;
    shaderData.setData(UnlitMaterial::_baseColorProp, newValue);
}

SampledTexture2DPtr UnlitMaterial::baseTexture() {
    return _baseTexture;
}

void UnlitMaterial::setBaseTexture(SampledTexture2DPtr newValue) {
    _baseTexture = newValue;
    shaderData.setData(UnlitMaterial::_baseTextureProp, newValue);
    
    if (newValue) {
        shaderData.enableMacro(HAS_BASE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_BASE_TEXTURE);
    }
}

const Vector4F& UnlitMaterial::tilingOffset() {
    return _tilingOffset;
}

void UnlitMaterial::setTilingOffset(const Vector4F &newValue) {
    _tilingOffset = newValue;
    shaderData.setData(UnlitMaterial::_tilingOffsetProp, newValue);
}

UnlitMaterial::UnlitMaterial(wgpu::Device& device) :
BaseMaterial(device, Shader::find("unlit")),
_baseColorProp(Shader::createProperty("u_baseColor", ShaderDataGroup::Material)),
_baseTextureProp(Shader::createProperty("u_baseTexture", ShaderDataGroup::Material)),
_tilingOffsetProp(Shader::createProperty("u_tilingOffset", ShaderDataGroup::Material)) {
    shaderData.enableMacro(OMIT_NORMAL);
    shaderData.enableMacro(NEED_TILINGOFFSET);
    
    shaderData.setData(_baseColorProp, _baseColor);
    shaderData.setData(_tilingOffsetProp, _tilingOffset);
}

}
