//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shader/state/render_state.h"

namespace vox {
void RenderState::apply(wgpu::ColorTargetState* colorTargetState,
                        wgpu::DepthStencilState* depthStencil,
                        wgpu::RenderPipelineDescriptor& pipelineDescriptor,
                        wgpu::RenderPassEncoder& encoder,
                        bool frontFaceInvert) {
    blendState.apply(colorTargetState, pipelineDescriptor.multisample, encoder);
    depthState.apply(depthStencil);
    stencilState.apply(depthStencil, encoder);
    rasterState.apply(pipelineDescriptor.primitive, depthStencil, frontFaceInvert);
}

}  // namespace vox
