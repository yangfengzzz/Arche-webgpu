//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_begin_normal_vert.h"
#include <fmt/core.h>

namespace vox {
WGSLBeginNormalVert::WGSLBeginNormalVert(const std::string& input, const std::string& output):
_input(input),
_output(output) {
}

void WGSLBeginNormalVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(HAS_NORMAL)) {
        source += fmt::format("var normal = {}.Normal;\n", _input);
        if (macros.contains(HAS_TANGENT) && macros.contains(HAS_NORMAL_TEXTURE)) {
            source += fmt::format("var tangent = {}.Tangent;\n", _input);
        }
    }
}

}
