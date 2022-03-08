//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sprite_debug_material.h"
#include <fmt/core.h>

namespace vox {
WGSLSpriteDebugVertex::WGSLSpriteDebugVertex(bool isSpotLight):
_isSpotLight(isSpotLight),
_lightFragDefine(),
_commonVert("VertexIn") {
}

void WGSLSpriteDebugVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter();
    auto outputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
        _lightFragDefine(encoder, macros);
        _commonVert(encoder, macros);
        
        encoder.addStruct("var<private> pos : array<vec2<f32>, 4> = array<vec2<f32>, 4>(\n"
                          "  vec2<f32>(-1.0, 1.0), vec2<f32>(1.0, 1.0), vec2<f32>(-1.0, -1.0), vec2<f32>(1.0, -1.0)\n"
                          ");\n");
        
        encoder.addInoutType("VertexIn", BuiltInType::VertexIndex, "vertexIndex", UniformType::U32);
        encoder.addInoutType("VertexIn", BuiltInType::InstanceIndex, "instanceIndex", UniformType::U32);
        
        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", 0, "localPos", UniformType::Vec2f32);
        encoder.addInoutType("VertexOut", 1, "color", UniformType::Vec3f32);
        
        encoder.addEntry({{"input", "VertexIn"}}, {"output", "VertexOut"}, [&](std::string &source){
            source += "output.localPos = pos[input.vertexIndex];\n";
            source += fmt::format("output.color = {}[input.instanceIndex].color;\n", _isSpotLight? "u_spotLight" : "u_pointLight");
            source += fmt::format("let worldPos = vec3<f32>(output.localPos, 0.0) * {}[input.instanceIndex].distance * 0.025;\n",
                                  _isSpotLight? "u_spotLight" : "u_pointLight");
            source += "\n";
            source += "// Generate a billboarded model view matrix\n";
            source += "var bbModelViewMatrix : mat4x4<f32>;\n";
            source += fmt::format("bbModelViewMatrix[3] = vec4<f32>({}[input.instanceIndex].position, 1.0);\n",
                                  _isSpotLight? "u_spotLight" : "u_pointLight");
            source += "bbModelViewMatrix = u_cameraData.u_viewMat * bbModelViewMatrix;\n"
            "bbModelViewMatrix[0][0] = 1.0;\n"
            "bbModelViewMatrix[0][1] = 0.0;\n"
            "bbModelViewMatrix[0][2] = 0.0;\n"
            "\n"
            "bbModelViewMatrix[1][0] = 0.0;\n"
            "bbModelViewMatrix[1][1] = 1.0;\n"
            "bbModelViewMatrix[1][2] = 0.0;\n"
            "\n"
            "bbModelViewMatrix[2][0] = 0.0;\n"
            "bbModelViewMatrix[2][1] = 0.0;\n"
            "bbModelViewMatrix[2][2] = 1.0;\n"
            "\n"
            "output.position = u_cameraData.u_projMat * bbModelViewMatrix * vec4<f32>(worldPos, 1.0);\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    WGSLEncoder::endCounter(outputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

//MARK: - WGSLSpriteDebugFragment
void WGSLSpriteDebugFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
        
        encoder.addInoutType("VertexOut", 0, "localPos", UniformType::Vec2f32);
        encoder.addInoutType("VertexOut", 1, "color", UniformType::Vec3f32);
        encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
        
        encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"},  [&](std::string &source){
            source += "let distToCenter = length(in.localPos);\n";
            source += "let fade = (1.0 - distToCenter) * (1.0 / (distToCenter * distToCenter));\n";
            source += "out.finalColor = vec4<f32>(in.color * fade, fade);\n";
        });
        encoder.flush();
    }
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

//MARK: - SpriteDebugMaterial
SpriteDebugMaterial::SpriteDebugMaterial(wgpu::Device& device, bool isSpotLight):
BaseMaterial(device, Shader::find(isSpotLight? "spotlight_sprite_debug": "pointlight_sprite_debug")) {
    setRenderFace(RenderFace::Double);
    setIsTransparent(true);
    setBlendMode(BlendMode::Additive);
}

}
