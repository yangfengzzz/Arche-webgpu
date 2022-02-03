//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_mobile_material_frag.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLMobileMaterialShare::WGSLMobileMaterialShare(const std::string& outputStructName) :
_outputStructName(outputStructName) {
}

void WGSLMobileMaterialShare::operator()(WGSLEncoder& encoder,
                                         const ShaderMacroCollection& macros, size_t counterIndex) {
    encoder.addUniformBinding("u_emissiveColor", UniformType::Vec4f32);
    encoder.addUniformBinding("u_diffuseColor", UniformType::Vec4f32);
    encoder.addUniformBinding("u_specularColor", UniformType::Vec4f32);
    encoder.addUniformBinding("u_shininess", UniformType::F32);
    encoder.addUniformBinding("u_normalIntensity", UniformType::F32);
    encoder.addUniformBinding("u_alphaCutoff", UniformType::F32);
    
    if (macros.contains(HAS_EMISSIVE_TEXTURE)) {
        encoder.addSampledTextureBinding("u_emissiveTexture", TextureType::Texture2Df32,
                                         "u_emissiveSampler", SamplerType::Sampler);
    }
    
    if (macros.contains(HAS_DIFFUSE_TEXTURE)) {
        encoder.addSampledTextureBinding("u_diffuseTexture", TextureType::Texture2Df32,
                                         "u_diffuseSampler", SamplerType::Sampler);
    }
    
    if (macros.contains(HAS_SPECULAR_TEXTURE)) {
        encoder.addSampledTextureBinding("u_specularTexture", TextureType::Texture2Df32,
                                         "u_specularSampler", SamplerType::Sampler);
    }
    
    if (macros.contains(HAS_NORMAL_TEXTURE)) {
        encoder.addSampledTextureBinding("u_normalTexture", TextureType::Texture2Df32,
                                         "u_normalSampler", SamplerType::Sampler);
    }
}

}
