//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_normal_vert.h"
#include <fmt/core.h>

namespace vox {
WGSLNormalVert::WGSLNormalVert(const std::string& input, const std::string& output):
_input(input),
_output(output) {
}

void WGSLNormalVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(HAS_NORMAL)) {
        if (macros.contains(HAS_TANGENT) && macros.contains(HAS_NORMAL_TEXTURE)) {
            source += "var normalW = normalize( mat3x3<f32>(u_normalMat) * normal.xyz );";
            source += "var tangentW = normalize( mat3x3<f32>(u_normalMat) * tangent.xyz );";
            source += "var bitangentW = cross( normalW, tangentW ) * tangent.w;";
            source += "v_TBN = mat3x3<f32>( tangentW, bitangentW, normalW );";
        } else {
            source += fmt::format("{}.v_normal = normalize(mat3x3<f32>(u_normalMat) * normal);\n", _output);
        }
    }
}

}
