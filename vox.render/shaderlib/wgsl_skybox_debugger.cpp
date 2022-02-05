//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_skybox_debugger.h"

namespace vox {
WGSLSkyboxDebuggerVertex::WGSLSkyboxDebuggerVertex():
_commonVert("VertexIn"),
_uvShare("VertexOut"),
_beginPositionVert("in", "out"),
_uvVert("in", "out"),
_positionVert("in", "out") {
}

void WGSLSkyboxDebuggerVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter();
    auto outputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
        _commonVert(encoder, macros);
        _uvShare(encoder, macros, outputStructCounter);
        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);

        encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string &source){
            _beginPositionVert(source, macros);
            _uvVert(source, macros);
            _positionVert(source, macros);
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    WGSLEncoder::endCounter(outputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

WGSLSkyboxDebuggerFragment::WGSLSkyboxDebuggerFragment():
_common(),
_uvShare("VertexOut") {
}

void WGSLSkyboxDebuggerFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
        _common(encoder, macros);
        _uvShare(encoder, macros, inputStructCounter);

        encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
        encoder.addSampledTextureBinding("u_baseTexture", TextureType::Texture2Df32, "u_baseSampler", SamplerType::Sampler);
        encoder.addUniformBinding("u_faceIndex", UniformType::I32);

        encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"},  [&](std::string &source){
            source += "var uv = in.v_uv;\n";
            source += "if (u_faceIndex == 2 || u_faceIndex == 3) {\n";
            source += "    uv.y = 1.0 - uv.y;\n";
            source += "}\n";
            
            source += "var baseColor = textureSample(u_baseTexture, u_baseSampler, in.v_uv);\n";
            source += "out.finalColor = vec4<f32>(baseColor.z, baseColor.y, baseColor.x, 1.0);\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
    
}

}
