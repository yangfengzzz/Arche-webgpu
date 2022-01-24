//
//  raster_state.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/24.
//

#ifndef raster_state_hpp
#define raster_state_hpp

#include <dawn/webgpu_cpp.h>

namespace vox {
/**
 * Raster state.
 */
struct RasterState {
    /** Specifies whether or not front- and/or back-facing polygons can be culled. */
    wgpu::CullMode cullMode = wgpu::CullMode::Front;
    /** The multiplier by which an implementation-specific value is multiplied with to create a constant depth offset. */
    float depthBias = 0;
    /** The scale factor for the variable depth offset for each polygon. */
    float slopeScaledDepthBias = 0;
    
    void apply(wgpu::PrimitiveState& primitive,
               wgpu::DepthStencilState * depthStencil,
               bool frontFaceInvert) {
        platformApply(primitive, depthStencil, frontFaceInvert);
    }
    
    void platformApply(wgpu::PrimitiveState& primitive,
                       wgpu::DepthStencilState * depthStencil,
                       bool frontFaceInvert);
};

}

#endif /* raster_state_hpp */
