//
//  stencil_state.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/24.
//

#ifndef stencil_state_hpp
#define stencil_state_hpp

#include <dawn/webgpu_cpp.h>

namespace vox {
/**
 * Stencil state.
 */
struct StencilState {
    /** Whether to enable stencil test. */
    bool enabled = false;
    /** Write the reference value of the stencil buffer. */
    uint32_t referenceValue = 0;
    /** Specifying a bit-wise mask that is used to AND the reference value and the stored stencil value when the test is done. */
    uint32_t mask = 0xff;
    /** Specifying a bit mask to enable or disable writing of individual bits in the stencil planes. */
    uint32_t writeMask = 0xff;
    /** The comparison function of the reference value of the front face of the geometry and the current buffer storage value. */
    wgpu::CompareFunction compareFunctionFront = wgpu::CompareFunction::Always;
    /** The comparison function of the reference value of the back of the geometry and the current buffer storage value. */
    wgpu::CompareFunction compareFunctionBack = wgpu::CompareFunction::Always;
    /** specifying the function to use for front face when both the stencil test and the depth test pass. */
    wgpu::StencilOperation passOperationFront = wgpu::StencilOperation::Keep;
    /** specifying the function to use for back face when both the stencil test and the depth test pass. */
    wgpu::StencilOperation passOperationBack = wgpu::StencilOperation::Keep;
    /** specifying the function to use for front face when the stencil test fails. */
    wgpu::StencilOperation failOperationFront = wgpu::StencilOperation::Keep;
    /** specifying the function to use for back face when the stencil test fails. */
    wgpu::StencilOperation failOperationBack = wgpu::StencilOperation::Keep;
    /** specifying the function to use for front face when the stencil test passes, but the depth test fails. */
    wgpu::StencilOperation zFailOperationFront = wgpu::StencilOperation::Keep;
    /** specifying the function to use for back face when the stencil test passes, but the depth test fails. */
    wgpu::StencilOperation zFailOperationBack = wgpu::StencilOperation::Keep;
    
    void apply(wgpu::DepthStencilState * depthStencil,
               wgpu::RenderPassEncoder &encoder) {
        platformApply(depthStencil, encoder);
    }
    
    void platformApply(wgpu::DepthStencilState * depthStencil,
                       wgpu::RenderPassEncoder &encoder);
};

}
#endif /* stencil_state_hpp */
