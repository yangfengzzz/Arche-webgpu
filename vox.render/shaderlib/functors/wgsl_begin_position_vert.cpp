//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_begin_position_vert.h"

namespace vox {
void WGSLBeginPositionVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    source += "var position = vec4<f32>( POSITION , 1.0 );\n";
}

}
