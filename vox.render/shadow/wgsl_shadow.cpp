//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_shadow.h"

namespace vox {
WGSLShadowVertex::WGSLShadowVertex():
_commonVert("VertexIn"),
_blendShapeInput("VertexIn"),
_beginPositionVert("in", "out"),
_blendShapeVert("in", "out"),
_skinningVert("in", "out") {
}

void WGSLShadowVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter();
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
        _commonVert(encoder, macros);
        _blendShapeInput(encoder, macros, inputStructCounter);
        encoder.addUniformBinding("u_shadowVPMat", UniformType::Mat4x4f32);
        
        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);

        encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string &source){
            _beginPositionVert(source, macros);
            _blendShapeVert(source, macros);
            _skinningVert(source, macros);
            
            source += "out.position = u_shadowVPMat * u_rendererData.u_modelMat * position;\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
