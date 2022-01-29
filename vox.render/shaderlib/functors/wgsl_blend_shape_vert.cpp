//
//  wgsl_blend_shape_vert.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "wgsl_blend_shape_vert.h"

namespace vox {
void WGSLBlendShapeVert::operator()(std::string& source, const ShaderMacroCollection& macros) {
    if (macros.contains(HAS_BLENDSHAPE)) {
        if (macros.contains(HAS_BASE_TEXTURE)) {
            
        } else {
            source += "position.xyz += POSITION_BS0 * u_blendShapeWeights[0];\n";
            source += "position.xyz += POSITION_BS1 * u_blendShapeWeights[1];\n";
            source += "position.xyz += POSITION_BS2 * u_blendShapeWeights[2];\n";
            source += "position.xyz += POSITION_BS3 * u_blendShapeWeights[3];\n";
            
            if (macros.contains(HAS_NORMAL) && macros.contains(HAS_BLENDSHAPE_NORMAL)) {
                source += "normal.xyz += NORMAL_BS0 * u_blendShapeWeights[0];\n";
                source += "normal.xyz += NORMAL_BS1 * u_blendShapeWeights[1];\n";
                source += "normal.xyz += NORMAL_BS2 * u_blendShapeWeights[2];\n";
                source += "normal.xyz += NORMAL_BS3 * u_blendShapeWeights[3];\n";
            }
            
            if (macros.contains(HAS_TANGENT) && macros.contains(HAS_BLENDSHAPE_TANGENT)) {
                source += "tangent.xyz += TANGENT_BS0 * u_blendShapeWeights[0];\n";
                source += "tangent.xyz += TANGENT_BS1 * u_blendShapeWeights[1];\n";
                source += "tangent.xyz += TANGENT_BS2 * u_blendShapeWeights[2];\n";
                source += "tangent.xyz += TANGENT_BS3 * u_blendShapeWeights[3];\n";
            }
        }
    }
}

}
