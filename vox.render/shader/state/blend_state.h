//
//  blend_state.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/24.
//

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
    
    BlendState();
    
    ~BlendState();
    
    /**
     * Apply the current blend state by comparing with the last blend state.
     */
    void apply(wgpu::FragmentState* fragment,
               wgpu::MultisampleState& multisample,
               wgpu::RenderPassEncoder &encoder) {
        platformApply(fragment, multisample, encoder);
    }
    
    void platformApply(wgpu::FragmentState* fragment,
                       wgpu::MultisampleState& multisample,
                       wgpu::RenderPassEncoder &encoder);
    
private:
    std::unique_ptr<wgpu::ColorTargetState> _colorTargetState{nullptr};
    std::unique_ptr<wgpu::BlendState> _blendState{nullptr};
};

}

#endif /* blend_state_hpp */
