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
        encoder.addInputType("Input", Attributes::UV_0, UniformType::Vec2f32);
    }
    
    if (macros.contains("HAS_SKIN")) {
        encoder.addInputType("Input", Attributes::Joints_0, UniformType::Vec4f32);
        encoder.addInputType("Input", Attributes::Weights_0, UniformType::Vec4f32);
    }
}

}
