//
//  depth_state.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/24.
//

#ifndef depth_state_hpp
#define depth_state_hpp

#include <dawn/webgpu_cpp.h>

namespace vox {
/**
 * Depth state.
 */
struct DepthState {
    /** Whether to enable the depth test. */
    bool enabled = true;
    /** Whether the depth value can be written.*/
    bool writeEnabled = true;
    /** Depth comparison function. */
    wgpu::CompareFunction compareFunction = wgpu::CompareFunction::Less;
    
    void apply(wgpu::DepthStencilState * depthStencil) {
        platformApply(depthStencil);
    }
    
    void platformApply(wgpu::DepthStencilState * depthStencil);
};

}
#endif /* depth_state_hpp */
