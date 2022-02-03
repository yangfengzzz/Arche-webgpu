//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_worldpos_vert.h"
#include <fmt/core.h>

namespace vox {
WGSLWorldPosVert::WGSLWorldPosVert(const std::string& input, const std::string& output):
_input(input),
_output(output) {
}

void WGSLWorldPosVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(NEED_WORLDPOS)) {
        source += "var temp_pos = u_modelMat * position;";
        source += fmt::format("{}.v_pos = temp_pos.xyz / temp_pos.w;", _output);
    }
}

}
