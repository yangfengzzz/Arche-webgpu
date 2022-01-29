//
//  wgsl_unlit.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "wgsl_unlit.h"

namespace vox {
WGSLUnlitVertex::WGSLUnlitVertex():
_commonVert("VertexIn"),
_blendShapeInput("VertexIn", WGSLEncoder::startCounter()),
_uvShare("VertexOut", WGSLEncoder::startCounter()),
_beginPositionVert(),
_blendShapeVert(),
_skinningVert(),
_uvVert(),
_positionVert() {
}

void WGSLUnlitVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    
}

WGSLUnlitFragment::WGSLUnlitFragment():
_common(),
_uvShare("VertexOut", WGSLEncoder::startCounter()) {
}

void WGSLUnlitFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    
}

}
