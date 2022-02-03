//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_blinn_phong.h"

namespace vox {
WGSLBlinnPhongVertex::WGSLBlinnPhongVertex():
_common(),
_commonVert("VertexIn"),
_blendShapeInput("VertexIn"),
_uvShare("VertexIn"),
_colorShare("VertexIn"),
_normalShare("VertexIn"),
_worldPosShare("VertexIn"),

_beginPositionVert("in", "out"),
_beginNormalVert("in", "out"),
_blendShapeVert("in", "out"),
_skinningVert("in", "out"),
_uvVert("in", "out"),
_colorVert("in", "out"),
_normalVert("in", "out"),
_worldPosVert("in", "out"),
_positionVert("in", "out") {
    
}

void WGSLBlinnPhongVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    
}

//MARK: - Frag
WGSLBlinnPhongFragment::WGSLBlinnPhongFragment():
_common(),
_commonFrag("VertexOut"),
_uvShare("VertexOut"),
_colorShare("VertexOut"),
_normalShare("VertexOut"),
_worldPosShare("VertexOut"),
_lightFragDefine("VertexOut"),
_mobileMaterialShare("VertexOut"),
_normalGet("VertexOut"),

_beginMobileFrag("in", "out"),
_beginViewDirFrag("in", "out"),
_mobileBlinnphoneFrag("in", "out") {
    
}

void WGSLBlinnPhongFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    
}

}
