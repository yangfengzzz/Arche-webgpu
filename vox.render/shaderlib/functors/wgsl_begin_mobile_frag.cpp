//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_begin_mobile_frag.h"
#include <fmt/core.h>

namespace vox {
WGSLBeginMobileFrag::WGSLBeginMobileFrag(const std::string& input, const std::string& output):
_input(input),
_output(output) {
}

void WGSLBeginMobileFrag::operator()(std::string& source, const ShaderMacroCollection& macros) {
    source += "var ambient = vec4<f32>(0.0);\n";
    source += "var emission = u_blinnPhongData.emissiveColor;\n";
    source += "var diffuse = u_blinnPhongData.baseColor;\n";
    source += "var specular = u_blinnPhongData.specularColor;\n";
    if (macros.contains(HAS_EMISSIVE_TEXTURE)) {
        source += fmt::format("var emissiveTextureColor = textureSample(u_emissiveTexture, u_emissiveSampler, {}.v_uv);\n", _input);
        source += "emission = emission * emissiveTextureColor;\n";
    }
    
    if (macros.contains(HAS_DIFFUSE_TEXTURE)) {
        source += fmt::format("var diffuseTextureColor = textureSample(u_diffuseTexture, u_diffuseSampler, {}.v_uv);\n", _input);
        source += "diffuse = diffuse * diffuseTextureColor;\n";
    }
    
    if (macros.contains(HAS_VERTEXCOLOR)) {
        source += fmt::format("diffuse = diffuse * {}.v_color;\n", _input);
    }
    
    if (macros.contains(HAS_SPECULAR_TEXTURE)) {
        source += fmt::format("var specularTextureColor = textureSample(u_specularTexture, u_specularSampler, {}.v_uv);\n", _input);
        source += "specular = specular * specularTextureColor;\n";
    }
    source += "ambient = vec4<f32>(u_envMapLight.diffuse * u_envMapLight.diffuseIntensity, 1.0) * diffuse;\n";
}

}
