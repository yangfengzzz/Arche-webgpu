//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_skybox.h"

namespace vox {
WGSLSkyboxVertex::WGSLSkyboxVertex():
_commonVert("VertexIn") {
}

void WGSLSkyboxVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
        _commonVert(encoder, macros);
        encoder.addUniformBinding("u_mvpNoscale", UniformType::Mat4x4f32);
        
        encoder.addInoutType("VertexOut", 0, "v_cubeUV", UniformType::Vec3f32);
        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);

        encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string &source){
            source += "out.v_cubeUV = in.Position.xyz;\n";
            source += "out.position = u_mvpNoscale * vec4<f32>( in.Position, 1.0 );\n";
            source += "out.position.z = out.position.w;\n";
        });
        encoder.flush();
    }
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

WGSLSkyboxFragment::WGSLSkyboxFragment():
_common() {
}

void WGSLSkyboxFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
        _common(encoder, macros);
        encoder.addSampledTextureBinding("u_cubeTexture", TextureType::Texture2Df32, "u_cubeSampler", SamplerType::Sampler);
        encoder.addInoutType("VertexOut", 0, "v_cubeUV", UniformType::Vec3f32);
        encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);

        encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"},  [&](std::string &source){
            source += "var textureColor = textureSample( u_cubeTexture, u_cubeSampler, v_cubeUV );\n";
            source += "out.finalColor = textureColor;\n";
        });
        encoder.flush();
    }
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
