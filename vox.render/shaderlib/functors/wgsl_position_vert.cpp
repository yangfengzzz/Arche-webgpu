//
//  wgsl_position_vert.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "wgsl_position_vert.h"

namespace vox {
void WGSLPositionVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    source += "out.position = u_MVPMat * position;\n";
}

}
