//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_color_vert.h"

#include <spdlog/fmt/fmt.h>

namespace vox {
WGSLColorVert::WGSLColorVert(const std::string& input, const std::string& output) : _input(input), _output(output) {}

void WGSLColorVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(HAS_VERTEXCOLOR)) {
        std::string formatTemplate = "{}.v_color = {}.COLOR_0;\n";
        source += fmt::format(formatTemplate, _output, _input);
    }
}

}  // namespace vox
