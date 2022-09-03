//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "blend_state.h"
#include "depth_state.h"
#include "raster_state.h"
#include "stencil_state.h"

namespace vox {
/**
 * Render state.
 */
struct RenderState {
    /** Blend state. */
    BlendState blendState = BlendState();
    /** Depth state. */
    DepthState depthState = DepthState();
    /** Stencil state. */
    StencilState stencilState = StencilState();
    /** Raster state. */
    RasterState rasterState = RasterState();

    void apply(wgpu::ColorTargetState* colorTargetState,
               wgpu::DepthStencilState* depthStencil,
               wgpu::RenderPipelineDescriptor& pipelineDescriptor,
               wgpu::RenderPassEncoder& encoder,
               bool frontFaceInvert);
};

}  // namespace vox