//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_normal_share.h"

#include <spdlog/fmt/fmt.h>

#include "vox.render/shaderlib/wgsl.h"

namespace vox {
WGSLNormalShare::WGSLNormalShare(const std::string& outputStructName) : _outputStructName(outputStructName) {}

void WGSLNormalShare::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros, size_t counterIndex) {
    if (macros.contains(HAS_NORMAL)) {
        if (macros.contains(HAS_TANGENT) && macros.contains(HAS_NORMAL_TEXTURE)) {
            encoder.addInoutType(_outputStructName, WGSLEncoder::getCounterNumber(counterIndex), "v_normalW",
                                 UniformType::Vec3f32);
            encoder.addInoutType(_outputStructName, WGSLEncoder::getCounterNumber(counterIndex), "v_tangentW",
                                 UniformType::Vec3f32);
            encoder.addInoutType(_outputStructName, WGSLEncoder::getCounterNumber(counterIndex), "v_bitangentW",
                                 UniformType::Vec3f32);
        } else {
            encoder.addInoutType(_outputStructName, WGSLEncoder::getCounterNumber(counterIndex), "v_normal",
                                 UniformType::Vec3f32);
        }
    }
}

}  // namespace vox
