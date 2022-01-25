//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unlit_material.h"

namespace vox {
Color UnlitMaterial::baseColor() {
    return std::any_cast<Color>(shaderData.getData(_baseColorProp));
}

void UnlitMaterial::setBaseColor(const Color &newValue) {
    shaderData.setData(UnlitMaterial::_baseColorProp, newValue);
}

std::shared_ptr<MTL::Texture> UnlitMaterial::baseTexture() {
    return std::any_cast<std::shared_ptr<MTL::Texture>>(shaderData.getData(_baseTextureProp));
}

void UnlitMaterial::setBaseTexture(std::shared_ptr<MTL::Texture> newValue) {
    shaderData.setData(UnlitMaterial::_baseTextureProp, newValue);
    
    if (newValue) {
        shaderData.enableMacro(HAS_BASE_TEXTURE);
    } else {
        shaderData.disableMacro(HAS_BASE_TEXTURE);
    }
}

Vector4F UnlitMaterial::tilingOffset() {
    return std::any_cast<Vector4F>(shaderData.getData(_tilingOffsetProp));
}

void UnlitMaterial::setTilingOffset(const Vector4F &newValue) {
    shaderData.setData(UnlitMaterial::_tilingOffsetProp, newValue);
}

UnlitMaterial::UnlitMaterial() :
BaseMaterial(Shader::find("unlit")),
_baseColorProp(Shader::createProperty("u_baseColor", ShaderDataGroup::Material)),
_baseTextureProp(Shader::createProperty("u_baseTexture", ShaderDataGroup::Material)),
_tilingOffsetProp(Shader::createProperty("u_tilingOffset", ShaderDataGroup::Material)) {
    shaderData.enableMacro(OMIT_NORMAL);
    shaderData.enableMacro(NEED_TILINGOFFSET);
    
    shaderData.setData(_baseColorProp, Color(1, 1, 1, 1));
    shaderData.setData(_tilingOffsetProp, Vector4F(1, 1, 0, 0));
}

}
