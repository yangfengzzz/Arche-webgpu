//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/wgsl/wgsl_shadow_vert.h"

#include <spdlog/fmt/fmt.h>

namespace vox {
WGSLShadowVert::WGSLShadowVert(std::string output) : _output(std::move(output)) {}

void WGSLShadowVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(SHADOW_MAP_COUNT)) {
        source += fmt::format("{}.view_pos = (u_cameraData.u_viewMat * u_rendererData.u_modelMat * position).xyz;\n",
                              _output);
    }
}

}  // namespace vox
