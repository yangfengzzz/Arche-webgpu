//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef blend_state_hpp
#define blend_state_hpp

#include "render_target_blend_state.h"
#include "color.h"

namespace vox {
/**
 * Blend state.
 */
struct BlendState {
    /** The blend state of the render target. */
    RenderTargetBlendState targetBlendState = RenderTargetBlendState();
    /** Constant blend color. */
    Color blendColor = Color(0, 0, 0, 0);
    /** Whether to use (Alpha-to-Coverage) technology. */
    bool alphaToCoverage = false;
        
    /**
     * Apply the current blend state by comparing with the last blend state.
     */
    void apply(wgpu::ColorTargetState* colorTargetState,
               wgpu::MultisampleState &multisample,
               wgpu::RenderPassEncoder &encoder) {
        platformApply(colorTargetState, multisample, encoder);
    }
    
    void platformApply(wgpu::ColorTargetState* colorTargetState,
                       wgpu::MultisampleState &multisample,
                       wgpu::RenderPassEncoder &encoder);
    
private:
    wgpu::BlendState _blendState;
};

}

#endif /* blend_state_hpp */
