//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_begin_position_vert.h"

#include <spdlog/fmt/fmt.h>

namespace vox {
WGSLBeginPositionVert::WGSLBeginPositionVert(const std::string& input, const std::string& output)
    : _input(input), _output(output) {
    _formatTemplate = "var position = vec4<f32>( {}.Position , 1.0 );\n";
    _formatTemplate = fmt::format(_formatTemplate, _input);
}

void WGSLBeginPositionVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    source += _formatTemplate;
}

}  // namespace vox
