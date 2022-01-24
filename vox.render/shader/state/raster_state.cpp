//
//  raster_state.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/24.
//

#include "raster_state.h"

namespace vox {
void RasterState::platformApply(wgpu::PrimitiveState& primitive,
                                wgpu::DepthStencilState * depthStencil,
                                bool frontFaceInvert) {
    primitive.cullMode = cullMode;
    if (frontFaceInvert) {
        primitive.frontFace = wgpu::FrontFace::CW;
    } else {
        primitive.frontFace = wgpu::FrontFace::CCW;
    }
    
    if (depthBias != 0 || slopeScaledDepthBias != 0) {
        depthStencil->depthBiasSlopeScale = slopeScaledDepthBias;
        depthStencil->depthBias = depthBias;
    }
}
}
