//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_unlit.h"

namespace vox {
WGSLUnlitVertex::WGSLUnlitVertex()
    : _commonVert("VertexIn"),
      _blendShapeInput("VertexIn"),
      _uvShare("VertexOut"),
      _beginPositionVert("in", "out"),
      _blendShapeVert("in", "out"),
      _skinningVert("in", "out"),
      _uvVert("in", "out"),
      _positionVert("in", "out") {}

void WGSLUnlitVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter();
    auto outputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
        _commonVert(encoder, macros);
        _blendShapeInput(encoder, macros, inputStructCounter);
        _uvShare(encoder, macros, outputStructCounter);
        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);

        encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string& source) {
            _beginPositionVert(source, macros);
            _blendShapeVert(source, macros);
            _skinningVert(source, macros);
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

WGSLUnlitFragment::WGSLUnlitFragment() : _common(), _uvShare("VertexOut") {}

void WGSLUnlitFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
        _common(encoder, macros);
        _uvShare(encoder, macros, inputStructCounter);
        encoder.addUniformBinding("u_baseColor", UniformType::Vec4f32);
        encoder.addUniformBinding("u_alphaCutoff", UniformType::F32);
        encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
        if (macros.contains(HAS_BASE_TEXTURE)) {
            encoder.addSampledTextureBinding("u_baseTexture", TextureType::Texture2Df32, "u_baseSampler",
                                             SamplerType::Sampler);
        }
        encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"}, [&](std::string& source) {
            source += "var baseColor = u_baseColor;\n";
            if (macros.contains(HAS_BASE_TEXTURE)) {
                source += "var textureColor = textureSample(u_baseTexture, u_baseSampler, in.v_uv);\n";
                source += "baseColor = baseColor * textureColor;\n";
            }
            if (macros.contains(NEED_ALPHA_CUTOFF)) {
                source += "if( baseColor.a < u_alphaCutoff ) {\n";
                source += "    discard;\n";
                source += "}\n";
            }
            source += "out.finalColor = baseColor;\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}  // namespace vox
