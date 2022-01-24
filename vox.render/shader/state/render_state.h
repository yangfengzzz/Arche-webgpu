//
//  render_state.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/24.
//

#ifndef render_state_hpp
#define render_state_hpp

#include "blend_state.h"
#include "raster_state.h"
#include "depth_state.h"
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
    
    void apply(wgpu::FragmentState* fragment,
               wgpu::DepthStencilState * depthStencil,
               wgpu::RenderPipelineDescriptor& pipelineDescriptor,
               wgpu::RenderPassEncoder &encoder,
               bool frontFaceInvert);
};

}

#endif /* render_state_hpp */
