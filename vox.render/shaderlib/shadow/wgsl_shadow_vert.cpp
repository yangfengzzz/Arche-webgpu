//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_shadow_vert.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLShadowVert::WGSLShadowVert(const std::pair<std::string, std::string>& outType):
_output(outType.first),
_outputStructName(outType.second) {
}

void WGSLShadowVert::operator()(WGSLEncoder& encoder,
                                const ShaderMacroCollection& macros, size_t counterIndex) {
    if (macros.contains(SHADOW_MAP_COUNT)) {
        encoder.addInoutType(_outputStructName, WGSLEncoder::getCounterNumber(counterIndex),
                             "view_pos", UniformType::Vec3f32);
    }
}

void WGSLShadowVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(SHADOW_MAP_COUNT)) {
        source += fmt::format("{}.view_pos = (u_rendererData.u_MVMat * position).xyz;\n", _output);
    }
}

}
