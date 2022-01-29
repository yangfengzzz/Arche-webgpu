//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_uv_vert.h"

namespace vox {
void WGSLUVVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(HAS_UV)) {
        source += "v_uv = TEXCOORD_0;\n";
    } else {
        // may need this calculate normal
        source += "v_uv = vec2<f32>( 0., 0. );\n";
    }
    
    if (macros.contains(NEED_TILINGOFFSET)) {
        source += "v_uv = v_uv * u_tilingOffset.xy + u_tilingOffset.zw;\n";
    }
}

}
