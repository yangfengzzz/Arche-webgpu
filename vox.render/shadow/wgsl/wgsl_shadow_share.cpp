//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_shadow_share.h"

namespace vox {
WGSLShadowShare::WGSLShadowShare(const std::string& outputStructName) :
_outputStructName(outputStructName) {
}

void WGSLShadowShare::operator()(WGSLEncoder& encoder,
                                 const ShaderMacroCollection& macros, size_t counterIndex) {
    if (macros.contains(SHADOW_MAP_COUNT)) {
        encoder.addInoutType(_outputStructName, WGSLEncoder::getCounterNumber(counterIndex),
                             "view_pos", UniformType::Vec3f32);
    }
}

}
