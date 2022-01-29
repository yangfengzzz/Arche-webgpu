//
//  uv_share.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "uv_share.h"
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
