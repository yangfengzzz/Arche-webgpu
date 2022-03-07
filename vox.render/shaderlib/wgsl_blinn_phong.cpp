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
_positionVert("in", "out"),

_shadowShare("VertexOut"),
_shadowVert("out"){
    
}

void WGSLBlinnPhongVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
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
        _shadowShare(encoder, macros, outputStructCounter);
        
        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);
        
        encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string &source){
            _beginPositionVert(source, macros);
            _beginNormalVert(source, macros);
            _blendShapeVert(source, macros);
            _skinningVert(source, macros);
            _uvVert(source, macros);
            _colorVert(source, macros);
            _normalVert(source, macros);
            _worldPosVert(source, macros);
            _positionVert(source, macros);
            
            _shadowVert(source, macros);
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    WGSLEncoder::endCounter(outputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

//MARK: - Frag
WGSLBlinnPhongFragment::WGSLBlinnPhongFragment():
_common(),
_commonFrag("VertexOut"),
_uvShare("VertexOut"),
_colorShare("VertexOut"),
_normalShare("VertexOut"),
_worldPosShare("VertexOut"),
_lightFragDefine(),
_mobileMaterialShare("VertexOut"),
_normalGet("VertexOut"),

_beginMobileFrag("in", "out"),
_beginViewDirFrag("in", "out"),
_mobileBlinnphoneFrag("in", "out"),

_shadowShare("VertexOut"),
_shadowFrag(),
_shadowCommon() {
    
}

void WGSLBlinnPhongFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
        _common(encoder, macros);
        _shadowCommon(encoder, macros);
        
        _commonFrag(encoder, macros);
        _shadowFrag(encoder, macros);
        
        _uvShare(encoder, macros, inputStructCounter);
        _colorShare(encoder, macros, inputStructCounter);
        _normalShare(encoder, macros, inputStructCounter);
        _worldPosShare(encoder, macros, inputStructCounter);
        _shadowShare(encoder, macros, inputStructCounter);

        _lightFragDefine(encoder, macros);
        _mobileMaterialShare(encoder, macros, inputStructCounter);
        _normalGet(encoder, macros, inputStructCounter);
        encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
        encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"}, [&](std::string &source){
            _beginMobileFrag(source, macros);
            _beginViewDirFrag(source, macros);
            _mobileBlinnphoneFrag(source, macros);
            _shadowFrag(source, macros);
            if (macros.contains(SHADOW_MAP_COUNT) || macros.contains(CUBE_SHADOW_MAP_COUNT)) {
                source += "diffuse = diffuse * shadow;\n";
                source += "specular = specular * shadow;\n";
            }
            
            source += "out.finalColor = emission + ambient + diffuse + specular;\n";
            source += "out.finalColor.a = diffuse.a;\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
