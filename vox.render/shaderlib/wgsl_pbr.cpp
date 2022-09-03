//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_pbr.h"

namespace vox {
WGSLPbrVertex::WGSLPbrVertex()
    : _common(),
      _commonVert("VertexIn"),
      _blendShapeInput("VertexIn"),
      _uvShare("VertexOut"),
      _colorShare("VertexOut"),
      _normalShare("VertexOut"),
      _worldPosShare("VertexOut"),

      _beginPositionVert("in", "out"),
      _beginNormalVert("in", "out"),
      _blendShapeVert("in", "out"),
      _skinningVert("in", "out"),
      _uvVert("in", "out"),
      _colorVert("in", "out"),
      _normalVert("in", "out"),
      _worldPosVert("in", "out"),
      _positionVert("in", "out") {}

void WGSLPbrVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter();
    auto outputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
        _common(encoder, macros);
        _commonVert(encoder, macros);
        _blendShapeInput(encoder, macros, inputStructCounter);
        _uvShare(encoder, macros, outputStructCounter);
        _colorShare(encoder, macros, outputStructCounter);
        _normalShare(encoder, macros, outputStructCounter);
        _worldPosShare(encoder, macros, outputStructCounter);
        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);

        encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string& source) {
            _beginPositionVert(source, macros);
            _beginNormalVert(source, macros);
            _blendShapeVert(source, macros);
            _skinningVert(source, macros);
            _uvVert(source, macros);
            _colorVert(source, macros);
            _normalVert(source, macros);
            _worldPosVert(source, macros);
            _positionVert(source, macros);
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    WGSLEncoder::endCounter(outputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

// MARK: - Frag
WGSLPbrFragment::WGSLPbrFragment(bool is_metallic_workflow)
    : _common(),
      _commonFrag("VertexOut"),
      _uvShare("VertexOut"),
      _colorShare("VertexOut"),
      _normalShare("VertexOut"),
      _worldPosShare("VertexOut"),
      _lightFragDefine(),
      _pbrFragDefine("VertexOut", is_metallic_workflow),
      _pbrHelper("VertexOut", is_metallic_workflow),
      _pbrFrag("in", "out", is_metallic_workflow) {}

void WGSLPbrFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
        _common(encoder, macros);
        _commonFrag(encoder, macros);
        _uvShare(encoder, macros, inputStructCounter);
        _colorShare(encoder, macros, inputStructCounter);
        _normalShare(encoder, macros, inputStructCounter);
        _worldPosShare(encoder, macros, inputStructCounter);
        _lightFragDefine(encoder, macros);
        _pbrFragDefine(encoder, macros, inputStructCounter);
        _pbrHelper(encoder, macros, inputStructCounter);
        encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
        encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"}, [&](std::string& source) {
            _pbrFrag(source, macros);
            source += "out.finalColor =vec4<f32>(totalRadiance, material.opacity);\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}  // namespace vox
