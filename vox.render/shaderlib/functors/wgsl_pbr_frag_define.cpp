//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_pbr_frag_define.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLPbrFragDefine::WGSLPbrFragDefine(const std::string& outputStructName, bool is_metallic_workflow) :
_is_metallic_workflow(is_metallic_workflow),
_outputStructName(outputStructName) {
    _pbrStruct = "struct PbrBaseData {\n";
    _pbrStruct += "  baseColor : vec4<f32>;\n";
    _pbrStruct += "  emissiveColor : vec4<f32>;\n";
    _pbrStruct += "  normalTextureIntensity : f32;\n";
    _pbrStruct += "  occlusionTextureIntensity : f32;\n";
    _pbrStruct += "};\n";
    
    if (_is_metallic_workflow) {
        _pbrStruct += "struct PbrData {\n";
        _pbrStruct += "  metallic : f32;\n";
        _pbrStruct += "  roughness : f32;\n";
        _pbrStruct += "};\n";
    } else {
        _pbrStruct += "struct PbrSpecularData {\n";
        _pbrStruct += "  specularColor : vec4<f32>;\n";
        _pbrStruct += "  glossiness : f32;\n";
        _pbrStruct += "};\n";
    }
}

void WGSLPbrFragDefine::operator()(WGSLEncoder& encoder,
                                   const ShaderMacroCollection& macros, size_t counterIndex) {
    encoder.addUniformBinding("u_alphaCutoff", UniformType::F32);
    encoder.addUniformBinding("u_pbrBaseData", "PbrBaseData");
    if (_is_metallic_workflow) {
        encoder.addUniformBinding("u_pbrData", "PbrData");
    } else {
        encoder.addUniformBinding("u_pbrSpecularData", "PbrSpecularData");
    }
    
    if (macros.contains(HAS_BASE_COLORMAP)) {
        encoder.addSampledTextureBinding("u_baseColorTexture", TextureType::Texture2Df32,
                                         "u_baseColorSampler", SamplerType::Sampler);
    }
    
    if (macros.contains(HAS_NORMAL_TEXTURE)) {
        encoder.addSampledTextureBinding("u_normalTexture", TextureType::Texture2Df32,
                                         "u_normalSampler", SamplerType::Sampler);
    }
    
    if (macros.contains(HAS_EMISSIVE_TEXTURE)) {
        encoder.addSampledTextureBinding("u_emissiveTexture", TextureType::Texture2Df32,
                                         "u_emissiveSampler", SamplerType::Sampler);
    }
    
    if (macros.contains(HAS_METALROUGHNESSMAP) && _is_metallic_workflow) {
        encoder.addSampledTextureBinding("u_metallicRoughnessTexture", TextureType::Texture2Df32,
                                         "u_metallicRoughnessSampler", SamplerType::Sampler);
    }
    
    if (macros.contains(HAS_SPECULARGLOSSINESSMAP) && !_is_metallic_workflow) {
        encoder.addSampledTextureBinding("u_specularGlossinessTexture", TextureType::Texture2Df32,
                                         "u_specularGlossinessSampler", SamplerType::Sampler);
    }
    
    if (macros.contains(HAS_OCCLUSIONMAP)) {
        encoder.addSampledTextureBinding("u_occlusionTexture", TextureType::Texture2Df32,
                                         "u_occlusionSampler", SamplerType::Sampler);
    }
    
    std::string structType = "struct ReflectedLight {\n";
    structType += "    directDiffuse: vec3<f32>;\n";
    structType += "    directSpecular: vec3<f32>;\n";
    structType += "    indirectDiffuse: vec3<f32>;\n";
    structType += "    indirectSpecular: vec3<f32>;\n";
    structType += "};\n";
    encoder.addStruct(structType);
    
    structType = "struct GeometricContext {\n";
    structType += "    position: vec3<f32>;\n";
    structType += "    normal: vec3<f32>;\n";
    structType += "    viewDir: vec3<f32>;\n";
    structType += "};\n";
    encoder.addStruct(structType);
    
    structType = "struct PhysicalMaterial {\n";
    structType += "    diffuseColor: vec3<f32>;\n";
    structType += "    roughness: f32;\n";
    structType += "    specularColor: vec3<f32>;\n";
    structType += "    opacity: f32;\n";
    structType += "};\n";
    encoder.addStruct(structType);
}

}
