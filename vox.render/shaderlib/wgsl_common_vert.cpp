//
//  wgsl_common_vert.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "wgsl_common_vert.h"
#include "wgsl.h"

namespace vox {
void WGSLCommonVert::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    if (macros.contains("HAS_UV")) {
        encoder.addInputType("Input", "@location(0) TEXCOORD_0: vec2<f32>;");        
    }
}

}
