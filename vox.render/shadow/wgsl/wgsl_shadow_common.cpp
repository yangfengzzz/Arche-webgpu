//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_shadow_common.h"
#include "filesystem.h"

namespace vox {
void WGSLShadowCommon::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    if (macros.contains(SHADOW_MAP_COUNT)) {
        int count = (int)*macros.macroConstant(SHADOW_MAP_COUNT);
        if (count == 1) {
            std::string shadowFunction = fs::readShader("shadow_single.wgsl");
            encoder.addStruct(shadowFunction);
        } else {
            std::string shadowFunction = fs::readShader("shadow.wgsl");
            encoder.addStruct(shadowFunction);
        }
    }
    
    if (macros.contains(CUBE_SHADOW_MAP_COUNT)) {
        std::string shadowFunction = fs::readShader("shadow_cube.wgsl");
        encoder.addStruct(shadowFunction);
    }
}

}
