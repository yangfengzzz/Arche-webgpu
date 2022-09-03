//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_uv_share.h"

#include <spdlog/fmt/fmt.h>

#include "vox.render/shaderlib/wgsl.h"

namespace vox {
WGSLUVShare::WGSLUVShare(const std::string& outputStructName) : _outputStructName(outputStructName) {}

void WGSLUVShare::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros, size_t counterIndex) {
    encoder.addInoutType(_outputStructName, WGSLEncoder::getCounterNumber(counterIndex), "v_uv", UniformType::Vec2f32);
}

}  // namespace vox
