//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_uv_vert.h"

#include <spdlog/fmt/fmt.h>

namespace vox {
WGSLUVVert::WGSLUVVert(const std::string& input, const std::string& output) : _input(input), _output(output) {}

void WGSLUVVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(HAS_UV)) {
        std::string formatTemplate = "{}.v_uv = {}.UV_0;\n";
        source += fmt::format(formatTemplate, _output, _input);
    } else {
        // may need this calculate normal
        std::string formatTemplate = "{}.v_uv = vec2<f32>( 0., 0. );\n";
        source += fmt::format(formatTemplate, _output);
    }

    if (macros.contains(NEED_TILINGOFFSET)) {
        std::string formatTemplate = "{}.v_uv = {}.v_uv * u_tilingOffset.xy + u_tilingOffset.zw;\n";
        source += fmt::format(formatTemplate, _output, _output);
    }
}

}  // namespace vox
