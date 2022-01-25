//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
