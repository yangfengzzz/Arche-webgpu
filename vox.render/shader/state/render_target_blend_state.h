//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef render_target_blend_state_h
#define render_target_blend_state_h

#include <dawn/webgpu_cpp.h>

namespace vox {
/**
 * The blend state of the render target.
 */
struct RenderTargetBlendState {
    /** Whether to enable blend. */
    bool enabled = false;
    /** color (RGB) blend operation. */
    wgpu::BlendOperation colorBlendOperation = wgpu::BlendOperation::Add;
    /** alpha (A) blend operation. */
    wgpu::BlendOperation alphaBlendOperation = wgpu::BlendOperation::Add;
    /** color blend factor (RGB) for source. */
    wgpu::BlendFactor sourceColorBlendFactor = wgpu::BlendFactor::One;
    /** alpha blend factor (A) for source. */
    wgpu::BlendFactor sourceAlphaBlendFactor = wgpu::BlendFactor::One;
    /** color blend factor (RGB) for destination. */
    wgpu::BlendFactor destinationColorBlendFactor = wgpu::BlendFactor::Zero;
    /** alpha blend factor (A) for destination. */
    wgpu::BlendFactor destinationAlphaBlendFactor = wgpu::BlendFactor::Zero;
    /** color mask. */
    wgpu::ColorWriteMask colorWriteMask = wgpu::ColorWriteMask::All;
};

}

#endif /* render_target_blend_state_h */
