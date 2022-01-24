//
//  stencil_state.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/24.
//

#include "stencil_state.h"

namespace vox {
void StencilState::platformApply(wgpu::DepthStencilState * depthStencil,
                                 wgpu::RenderPassEncoder &encoder) {
    if (enabled) {
        // apply stencil func.
        encoder.SetStencilReference(referenceValue);
        depthStencil->stencilReadMask = mask;
        depthStencil->stencilFront.compare = compareFunctionFront;
        depthStencil->stencilBack.compare = compareFunctionBack;
    }
    
    // apply stencil operation.
    depthStencil->stencilFront.failOp = failOperationFront;
    depthStencil->stencilFront.depthFailOp = zFailOperationFront;
    depthStencil->stencilFront.passOp = passOperationFront;
    
    depthStencil->stencilBack.failOp = failOperationBack;
    depthStencil->stencilBack.depthFailOp = zFailOperationBack;
    depthStencil->stencilBack.passOp = passOperationBack;
    
    // apply write mask.
    depthStencil->stencilWriteMask = writeMask;
}

}
