//
//  wgsl_position_vert.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "wgsl_position_vert.h"
#include <fmt/core.h>

namespace vox {
WGSLPositionVert::WGSLPositionVert(const std::string& input, const std::string& output):
_input(input),
_output(output) {
    _formatTemplate = "{}.position = u_MVPMat * position;\n";
    _formatTemplate = fmt::format(_formatTemplate, _output);
}

void WGSLPositionVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    source += _formatTemplate;
}

}
