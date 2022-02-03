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
                          " var color : vec3<f32>;\n"
                          " var direction : vec3<f32>;\n"
                          "};\n");
        encoder.addUniformBinding("u_directLight", "DirectLight");
    }
    
    if (macros.contains(POINT_LIGHT_COUNT)) {
        encoder.addStruct("struct PointLight {\n"
                          " var color : vec3<f32>;\n"
                          " var position : vec3<f32>;\n"
                          " var distance : f32;\n"
                          "};\n");
        encoder.addUniformBinding("u_pointLight", "PointLight");
    }
    
    if (macros.contains(SPOT_LIGHT_COUNT)) {
        encoder.addStruct("struct SpotLight {\n"
                          " var color : vec3<f32>;\n"
                          " var position : vec3<f32>;\n"
                          " var direction : vec3<f32>;\n"
                          " var distance : f32;\n"
                          " var angleCos : f32;\n"
                          " var penumbraCos : f32;\n"
                          "};\n");
        encoder.addUniformBinding("u_spotLight", "SpotLight");
    }
    
    encoder.addStruct("struct EnvMapLight {\n"
                      " var diffuse : vec3<f32>;\n"
                      " var mipMapLevel : f32;\n"
                      " var diffuseIntensity : f32;\n"
                      " var specularIntensity : f32;\n"
                      "};\n");
    encoder.addUniformBinding("u_envMapLight", "EnvMapLight");
    
    if (macros.contains(HAS_SH)) {
        encoder.addUniformBinding("u_env_sh", UniformType::Vec3f32);
    }
    
    if (macros.contains(HAS_SPECULAR_ENV)) {
        encoder.addSampledTextureBinding("u_env_specularTexture", TextureType::TextureCubef32,
                                         "u_env_specularSampler", SamplerType::Sampler);
        
    }
}

}
