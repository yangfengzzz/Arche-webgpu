//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_uv_share.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLUVShare::WGSLUVShare(const std::string& outputStructName, size_t counterIndex) :
_outputStructName(outputStructName),
_counterIndex(counterIndex) {
}

void WGSLUVShare::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addInoutType(_outputStructName, WGSLEncoder::getCounterNumber(_counterIndex),
                         "v_uv", UniformType::Vec2f32);
}

}
