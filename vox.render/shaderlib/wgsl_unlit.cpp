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
    _source.clear();
    _bindGroupInfo.clear();
    
    auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
    _commonVert(encoder, macros);
    _blendShapeInput(encoder, macros);
    _uvShare(encoder, macros);
    encoder.addEntry({{"in", "VertexIn"}}, "VertexOut", [&](std::string &source){
        _beginPositionVert(source, macros);
        _blendShapeVert(source, macros);
        _skinningVert(source, macros);
        _uvVert(source, macros);
        _positionVert(source, macros);
    });
    encoder.flush();
    
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

WGSLUnlitFragment::WGSLUnlitFragment():
_common(),
_uvShare("VertexOut", WGSLEncoder::startCounter()) {
}

void WGSLUnlitFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    
    auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
    _common(encoder, macros);
    _uvShare(encoder, macros);
    encoder.addUniformBinding("u_baseColor", UniformType::Vec4f32);
    encoder.addUniformBinding("u_alphaCutoff", UniformType::F32);
    if (macros.contains(HAS_BASE_TEXTURE)) {
        // TODO
    }
    encoder.addEntry({{"in", "VertexOut"}}, "FragmentOut",  [&](std::string &source){
        source += "vec4 baseColor = u_baseColor;\n";
        if (macros.contains(HAS_BASE_TEXTURE)) {
            source += "vec4 textureColor = texture2D(u_baseTexture, v_uv);\n";
            source += "baseColor *= textureColor;\n";
        }
        if (macros.contains(NEED_ALPHA_CUTOFF)) {
            source += "if( baseColor.a < u_alphaCutoff ) {\n";
            source += "    discard;\n";
            source += "}\n";
        }
        
        source += "return output;\n ";;
    });
    encoder.flush();
    
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
    
}

}
