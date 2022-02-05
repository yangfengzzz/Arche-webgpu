//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_light_frag_define.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLLightFragDefine::WGSLLightFragDefine(const std::string& outputStructName) :
_outputStructName(outputStructName) {
}

void WGSLLightFragDefine::operator()(WGSLEncoder& encoder,
                                     const ShaderMacroCollection& macros, size_t counterIndex) {
    if (macros.contains(DIRECT_LIGHT_COUNT)) {
        encoder.addStruct("struct DirectLight {\n"
                          "  color : vec3<f32>;\n"
                          "  direction : vec3<f32>;\n"
                          "};\n");
        encoder.addUniformBinding("u_directLight", fmt::format("array<DirectLight, {}>", (int)*macros.macroConstant(DIRECT_LIGHT_COUNT)) );
    }
    
    if (macros.contains(POINT_LIGHT_COUNT)) {
        encoder.addStruct("struct PointLight {\n"
                          "  color : vec3<f32>;\n"
                          "  position : vec3<f32>;\n"
                          "  distance : f32;\n"
                          "};\n");
        encoder.addUniformBinding("u_pointLight", fmt::format("array<PointLight, {}>", (int)*macros.macroConstant(POINT_LIGHT_COUNT)) );
    }
    
    if (macros.contains(SPOT_LIGHT_COUNT)) {
        encoder.addStruct("struct SpotLight {\n"
                          "  color : vec3<f32>;\n"
                          "  position : vec3<f32>;\n"
                          "  direction : vec3<f32>;\n"
                          "  distance : f32;\n"
                          "  angleCos : f32;\n"
                          "  penumbraCos : f32;\n"
                          "};\n");
        encoder.addUniformBinding("u_spotLight", fmt::format("array<SpotLight, {}>", (int)*macros.macroConstant(SPOT_LIGHT_COUNT)) );
    }
    
    encoder.addStruct("struct EnvMapLight {\n"
                      "  diffuse : vec3<f32>;\n"
                      "  mipMapLevel : f32;\n"
                      "  diffuseIntensity : f32;\n"
                      "  specularIntensity : f32;\n"
                      "};\n");
    encoder.addUniformBinding("u_envMapLight", "EnvMapLight");
    
    if (macros.contains(HAS_SH)) {
        encoder.addUniformBinding("u_env_sh", UniformType::Vec3f32);
    }
    
    if (macros.contains(HAS_DIFFUSE_ENV)) {
        encoder.addSampledTextureBinding("u_env_diffuseTexture", TextureType::TextureCubef32,
                                         "u_env_diffuseSampler", SamplerType::Sampler);
    }
    
    if (macros.contains(HAS_SPECULAR_ENV)) {
        encoder.addSampledTextureBinding("u_env_specularTexture", TextureType::TextureCubef32,
                                         "u_env_specularSampler", SamplerType::Sampler);
    }
}

}
