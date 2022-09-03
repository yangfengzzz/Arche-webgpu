//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_position_vert.h"

#include <spdlog/fmt/fmt.h>

namespace vox {
WGSLPositionVert::WGSLPositionVert(const std::string& input, const std::string& output)
    : _input(input), _output(output) {
    _formatTemplate = "{}.position = u_cameraData.u_VPMat * u_rendererData.u_modelMat * position;\n";
    _formatTemplate = fmt::format(_formatTemplate, _output);
}

void WGSLPositionVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    source += _formatTemplate;
}

}  // namespace vox
