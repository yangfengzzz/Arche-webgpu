//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/material/base_material.h"

#include "vox.render/shader/internal_variant_name.h"

namespace vox {
bool BaseMaterial::isTransparent() const { return _isTransparent; }

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
        renderQueueType = _alphaCutoff > 0 ? RenderQueueType::AlphaTest : RenderQueueType::Opaque;
    }
}

float BaseMaterial::alphaCutoff() const { return _alphaCutoff; }

void BaseMaterial::setAlphaCutoff(float newValue) {
    _alphaCutoff = newValue;
    shaderData.setData(BaseMaterial::_alphaCutoffProp, newValue);

    if (newValue > 0) {
        shaderData.addDefine(NEED_ALPHA_CUTOFF);
        renderQueueType = _isTransparent ? RenderQueueType::Transparent : RenderQueueType::AlphaTest;
    } else {
        shaderData.removeDefine(NEED_ALPHA_CUTOFF);
        renderQueueType = _isTransparent ? RenderQueueType::Transparent : RenderQueueType::Opaque;
    }
}

const Vector4F &BaseMaterial::tilingOffset() { return _tilingOffset; }

void BaseMaterial::setTilingOffset(const Vector4F &newValue) {
    _tilingOffset = newValue;
    shaderData.setData(BaseMaterial::_tilingOffsetProp, newValue);
}

const RenderFace &BaseMaterial::renderFace() { return _renderFace; }

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

const BlendMode &BaseMaterial::blendMode() { return _blendMode; }

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
            target.destinationColorBlendFactor = wgpu::BlendFactor::One;
            target.sourceAlphaBlendFactor = wgpu::BlendFactor::One;
            target.destinationAlphaBlendFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
            target.alphaBlendOperation = wgpu::BlendOperation::Add;
            target.colorBlendOperation = wgpu::BlendOperation::Add;
            break;
    }
}

BaseMaterial::BaseMaterial(wgpu::Device &device, const std::string &name) : Material(device, name) {
    setBlendMode(BlendMode::Normal);
    shaderData.setData(_alphaCutoffProp, 0.0f);
    shaderData.setData(_tilingOffsetProp, _tilingOffset);
    shaderData.addDefine(NEED_TILINGOFFSET);
}

const std::string BaseMaterial::_alphaCutoffProp = "u_alphaCutoff";
const std::string BaseMaterial::_tilingOffsetProp = "u_tilingOffset";

const std::string BaseMaterial::_baseTextureProp = "u_baseColorTexture";
const std::string BaseMaterial::_baseSamplerProp = "u_baseColorSampler";

const std::string BaseMaterial::_normalTextureProp = "u_normalTexture";
const std::string BaseMaterial::_normalSamplerProp = "u_normalSampler";

const std::string BaseMaterial::_emissiveTextureProp = "u_emissiveTexture";
const std::string BaseMaterial::_emissiveSamplerProp = "u_emissiveSampler";

}  // namespace vox
