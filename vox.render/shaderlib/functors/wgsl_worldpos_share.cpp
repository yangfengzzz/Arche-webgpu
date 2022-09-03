//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_worldpos_share.h"

#include <spdlog/fmt/fmt.h>

#include "vox.render/shaderlib/wgsl.h"

namespace vox {
WGSLWorldPosShare::WGSLWorldPosShare(const std::string& outputStructName) : _outputStructName(outputStructName) {}

void WGSLWorldPosShare::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros, size_t counterIndex) {
    if (macros.contains(NEED_WORLDPOS)) {
        encoder.addInoutType(_outputStructName, WGSLEncoder::getCounterNumber(counterIndex), "v_pos",
                             UniformType::Vec3f32);
    }
}

}  // namespace vox
