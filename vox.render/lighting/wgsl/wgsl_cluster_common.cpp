//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_cluster_common.h"

namespace vox {
void WGSLForwardPlusUniforms::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct(
            "struct ForwardPlusUniforms {\n"
            "  matrix : mat4x4<f32>;\n"
            "  inverseMatrix : mat4x4<f32>;\n"
            "  outputSize : vec2<f32>;\n"
            "  zNear : f32;\n"
            "  zFar : f32;\n"
            "  viewMatrix : mat4x4<f32>;\n"
            "};\n");
    encoder.addUniformBinding("u_cluster_uniform", "ForwardPlusUniforms");
}

}  // namespace vox
