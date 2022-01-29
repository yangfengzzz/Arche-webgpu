//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "base_material.h"

namespace vox {
bool BaseMaterial::isTransparent() {
    return _isTransparent;
}

void BaseMaterial::setIsTransparent(bool newValue) {
    if (newValue == _isTransparent) {
        return;
    }
    _isTransparent = newValue;
    
    auto &depthState = renderState.depthState;
    auto &targetBlendState = renderState.blendState.targetBlendState;
    
    if (newValue) {
        targetBlendState.enabled = true;
        depthState.writeEnabled = false;
        renderQueueType = RenderQueueType::Transparent;
    } else {
        targetBlendState.enabled = false;
        depthState.writeEnabled = true;
        renderQueueType = _alphaCutoff? RenderQueueType::AlphaTest : RenderQueueType::Opaque;
    }
}

float BaseMaterial::alphaCutoff() {
    return _alphaCutoff;
}

void BaseMaterial::setAlphaCutoff(float newValue) {
    _alphaCutoff = newValue;
    shaderData.setData(BaseMaterial::_alphaCutoffProp, newValue);
    
    if (newValue > 0) {
        shaderData.enableMacro(NEED_ALPHA_CUTOFF);
        renderQueueType = _isTransparent ? RenderQueueType::Transparent : RenderQueueType::AlphaTest;
    } else {
        shaderData.disableMacro(NEED_ALPHA_CUTOFF);
        renderQueueType = _isTransparent ? RenderQueueType::Transparent : RenderQueueType::Opaque;
    }
}

const RenderFace &BaseMaterial::renderFace() {
    return _renderFace;
}

void BaseMaterial::setRenderFace(const RenderFace &newValue) {
    _renderFace = newValue;
    
    switch (newValue) {
        case RenderFace::Front:
            renderState.rasterState.cullMode = wgpu::CullMode::Back;
            break;
        case RenderFace::Back:
            renderState.rasterState.cullMode = wgpu::CullMode::Front;
            break;
        case RenderFace::Double:
            renderState.rasterState.cullMode = wgpu::CullMode::None;
            break;
    }
}

const BlendMode &BaseMaterial::blendMode() {
    return _blendMode;
}

void BaseMaterial::setBlendMode(const BlendMode &newValue) {
    _blendMode = newValue;
    
    auto &target = renderState.blendState.targetBlendState;
    
    switch (newValue) {
        case BlendMode::Normal:
            target.sourceColorBlendFactor = wgpu::BlendFactor::SrcAlpha;
            target.destinationColorBlendFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
            target.sourceAlphaBlendFactor = wgpu::BlendFactor::One;
            target.destinationAlphaBlendFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
            target.alphaBlendOperation = wgpu::BlendOperation::Add;
            target.colorBlendOperation = wgpu::BlendOperation::Add;
            break;
        case BlendMode::Additive:
            target.sourceColorBlendFactor = wgpu::BlendFactor::SrcAlpha;
            target.destinationColorBlendFactor =wgpu::BlendFactor::One;
            target.sourceAlphaBlendFactor = wgpu::BlendFactor::One;
            target.destinationAlphaBlendFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
            target.alphaBlendOperation = wgpu::BlendOperation::Add;
            target.colorBlendOperation = wgpu::BlendOperation::Add;
            break;
    }
}

BaseMaterial::BaseMaterial(wgpu::Device& device, Shader *shader) :
Material(device, shader),
_alphaCutoffProp(Shader::createProperty("u_alphaCutoff", ShaderDataGroup::Material)) {
    setBlendMode(BlendMode::Normal);
    shaderData.setData(_alphaCutoffProp, 0.0f);
}

}
