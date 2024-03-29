//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shader/state/blend_state.h"

namespace vox {
void BlendState::platformApply(wgpu::ColorTargetState *colorTargetState,
                               wgpu::MultisampleState &multisample,
                               wgpu::RenderPassEncoder &encoder) {
    const auto enabled = targetBlendState.enabled;
    const auto colorBlendOperation = targetBlendState.colorBlendOperation;
    const auto alphaBlendOperation = targetBlendState.alphaBlendOperation;
    const auto sourceColorBlendFactor = targetBlendState.sourceColorBlendFactor;
    const auto destinationColorBlendFactor = targetBlendState.destinationColorBlendFactor;
    const auto sourceAlphaBlendFactor = targetBlendState.sourceAlphaBlendFactor;
    const auto destinationAlphaBlendFactor = targetBlendState.destinationAlphaBlendFactor;
    const auto colorWriteMask = targetBlendState.colorWriteMask;

    if (enabled && colorTargetState) {
        colorTargetState->blend = &_blendState;
    } else {
        if (colorTargetState) {
            colorTargetState->blend = nullptr;
        }
    }

    if (enabled) {
        // apply blend factor.
        _blendState.color.srcFactor = sourceColorBlendFactor;
        _blendState.color.dstFactor = destinationColorBlendFactor;
        _blendState.alpha.srcFactor = sourceAlphaBlendFactor;
        _blendState.alpha.dstFactor = destinationAlphaBlendFactor;

        // apply blend operation.
        _blendState.color.operation = colorBlendOperation;
        _blendState.alpha.operation = alphaBlendOperation;

        // apply blend color.
        encoder.SetBlendConstant(reinterpret_cast<wgpu::Color *>(&blendColor));

        // apply color mask.
        if (colorTargetState) {
            colorTargetState->writeMask = colorWriteMask;
        }
    }

    multisample.alphaToCoverageEnabled = alphaToCoverage;
}

}  // namespace vox
