//
//  render_state.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/24.
//

#include "render_state.h"

namespace vox {
void RenderState::apply(wgpu::FragmentState* fragment,
                        wgpu::DepthStencilState * depthStencil,
                        wgpu::RenderPipelineDescriptor& pipelineDescriptor,
                        wgpu::RenderPassEncoder &encoder,
                        bool frontFaceInvert) {
    blendState.apply(fragment, pipelineDescriptor.multisample, encoder);
    depthState.apply(depthStencil);
    stencilState.apply(depthStencil, encoder);
    rasterState.apply(pipelineDescriptor.primitive, depthStencil, frontFaceInvert);
}

}
