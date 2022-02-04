//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_begin_viewdir_frag.h"
#include <fmt/core.h>

namespace vox {
WGSLBeginViewDirFrag::WGSLBeginViewDirFrag(const std::string& input, const std::string& output):
_input(input),
_output(output) {
}

void WGSLBeginViewDirFrag::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(NEED_WORLDPOS)) {
        source += fmt::format("var V = normalize(u_cameraPos - {}.v_pos);\n", _input);
    }
}

}
