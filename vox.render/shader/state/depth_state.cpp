//
//  depth_state.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/24.
//

#include "depth_state.h"

namespace vox {
void DepthState::platformApply(wgpu::DepthStencilState * depthStencil) {
    if (enabled) {
        // apply compare func.
        depthStencil->depthCompare = compareFunction;
        
        // apply write enabled.
        depthStencil->depthWriteEnabled = writeEnabled;
    }
}

}
