//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_normal_vert.h"

#include <spdlog/fmt/fmt.h>

namespace vox {
WGSLNormalVert::WGSLNormalVert(const std::string& input, const std::string& output) : _input(input), _output(output) {}

void WGSLNormalVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(HAS_NORMAL)) {
        if (macros.contains(HAS_TANGENT) && macros.contains(HAS_NORMAL_TEXTURE)) {
            source +=
                    "var normalW = normalize( mat3x3<f32>(u_rendererData.u_normalMat[0].xyz, "
                    "u_rendererData.u_normalMat[1].xyz, u_rendererData.u_normalMat[2].xyz) * normal.xyz );\n";
            source +=
                    "var tangentW = normalize( mat3x3<f32>(u_rendererData.u_normalMat[0].xyz, "
                    "u_rendererData.u_normalMat[1].xyz, u_rendererData.u_normalMat[2].xyz) * tangent.xyz );\n";
            source += "var bitangentW = cross( normalW, tangentW ) * tangent.w;\n";
            source += fmt::format("{}.v_normalW = normalW;\n", _output);
            source += fmt::format("{}.v_tangentW = tangentW;\n", _output);
            source += fmt::format("{}.v_bitangentW = bitangentW;\n", _output);
        } else {
            source += fmt::format(
                    "{}.v_normal = normalize(mat3x3<f32>(u_rendererData.u_normalMat[0].xyz, "
                    "u_rendererData.u_normalMat[1].xyz, u_rendererData.u_normalMat[2].xyz) * normal);\n",
                    _output);
        }
    }
}

}  // namespace vox
