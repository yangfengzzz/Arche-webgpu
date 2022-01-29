//
//  wgsl_common_vert.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "wgsl_common_vert.h"
#include "wgsl.h"

namespace vox {
WGSLCommonVert::WGSLCommonVert(WGSL* source):
_source(source),
_uPMatirx(source, "u_projMat", UniformType::Mat4x4f32),
_uMVMatrix(source, "u_MVMat", UniformType::Mat4x4f32) {
}

void WGSLCommonVert::operator()(const ShaderMacroCollection& macros) {
    if (macros.contains("HAS_UV")) {
        _source->addInputType("@location(0) TEXCOORD_0: vec2<f32>;");
    }
}

}
