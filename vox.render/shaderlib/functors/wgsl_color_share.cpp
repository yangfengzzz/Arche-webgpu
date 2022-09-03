//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_color_share.h"

#include <spdlog/fmt/fmt.h>

#include "vox.render/shaderlib/wgsl.h"

namespace vox {
WGSLColorShare::WGSLColorShare(const std::string& outputStructName) : _outputStructName(outputStructName) {}

void WGSLColorShare::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros, size_t counterIndex) {
    if (macros.contains(HAS_VERTEXCOLOR)) {
        encoder.addInoutType(_outputStructName, WGSLEncoder::getCounterNumber(counterIndex), "v_color",
                             UniformType::Vec4f32);
    }
}

}  // namespace vox
