//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_shadow_frag.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLShadowFrag::WGSLShadowFrag(const std::pair<std::string, std::string>& outType):
_output(outType.first),
_outputStructName(outType.second) {
    {
        _shadowDataStruct += "struct ShadowData {\n";
        /// Shadow bias.
        _shadowDataStruct += " bias:f32;\n";
        /// Shadow intensity, the larger the value, the clearer and darker the shadow.
        _shadowDataStruct += " intensity:f32;\n";
        /// Pixel range used for shadow PCF interpolation.
        _shadowDataStruct += " radius:f32;\n";
        /// Alignment
        _shadowDataStruct += " dump:f32;\n";
        /// Light view projection matrix.(cascade)
        _shadowDataStruct += " vp: array<mat4x4<f32>, 4>;\n";
        /// Light cascade depth.
        _shadowDataStruct += " cascadeSplits: array<f32, 4>;\n";
        _shadowDataStruct += "};\n";
    }
    {
        _cubeShadowDataStruct += "struct CubeShadowData {\n";
        /// Shadow bias.
        _cubeShadowDataStruct += " bias: f32;\n";
        /// Shadow intensity, the larger the value, the clearer and darker the shadow.
        _cubeShadowDataStruct += " intensity:f32;\n";
        /// Pixel range used for shadow PCF interpolation.
        _cubeShadowDataStruct += " radius:f32;\n";
        /// Alignment
        _cubeShadowDataStruct += " dump:f32;\n";
        /// Light view projection matrix.
        _cubeShadowDataStruct += " vp: array<mat4x4<f32>, 6>;\n";
        _cubeShadowDataStruct += " lightPos: vec3<f32>;\n";
        _cubeShadowDataStruct += "};\n";
    }
}

void WGSLShadowFrag::operator()(WGSLEncoder& encoder,
                                const ShaderMacroCollection& macros, size_t counterIndex) {
    if (macros.contains(SHADOW_MAP_COUNT)) {
        encoder.addStruct(_shadowDataStruct);
        encoder.addUniformBinding("u_shadowData", "ShadowData");
        encoder.addSampledTextureBinding("u_shadowMap", TextureType::TextureDepth2DArray, "u_shadowSampler", SamplerType::SamplerComparison);
    }
    
    if (macros.contains(CUBE_SHADOW_MAP_COUNT)) {
        encoder.addStruct(_cubeShadowDataStruct);
        encoder.addUniformBinding("u_cubeShadowData", "CubeShadowData");
        encoder.addSampledTextureBinding("u_cubeShadowMap", TextureType::TextureDepthCubeArray, "u_cubeShadowSampler", SamplerType::SamplerComparison);
    }
}

void WGSLShadowFrag::operator()(std::string& source, const ShaderMacroCollection& macros) {
    source += "var shadow:f32 = 0;\n";
    source += "var totalShadow:f32 = 0;\n";
    
    if (macros.contains(SHADOW_MAP_COUNT)) {
        source += "{\n";
        source += "var i:i32 = 0;\n";
        source += "loop {\n";
        source += fmt::format("if (i >= {}) {{ break; }}\n", (int)*macros.macroConstant(CUBE_SHADOW_MAP_COUNT));
        
        source += "shadow += filterPCF(in.v_pos, in.view_pos, u_shadowMap, u_shadowData, i);\n";
        // source += "shadow += textureProj(in.v_pos, in.view_pos, float2(0), u_shadowMap, u_shadowData, i);\n";

        source += "i = i + 1;\n";
        source += "}\n";
        source += "}\n";
        
        source += "totalShadow += shadowMapCount;\n";
    }
    
    if (macros.contains(CUBE_SHADOW_MAP_COUNT)) {
        source += "{\n";
        source += "var i:i32 = 0;\n";
        source += "loop {\n";
        source += fmt::format("if (i >= {}) {{ break; }}\n", (int)*macros.macroConstant(CUBE_SHADOW_MAP_COUNT));
        
        // source += "shadow += cubeFilterPCF(in.v_pos, in.view_pos, u_cubeShadowMap, u_cubeShadowData, i);\n"; // too expensive
        source += "shadow += cubeTextureProj(in.v_pos, in.view_pos, vec2<f32>(0.0, 0.0), u_cubeShadowMap, u_cubeShadowData, i);\n";
        
        source += "i = i + 1;\n";
        source += "}\n";
        source += "}\n";
        
        source += "totalShadow += cubeShadowMapCount;\n";
    }
    
    if (macros.contains(SHADOW_MAP_COUNT) || macros.contains(CUBE_SHADOW_MAP_COUNT)) {
        source += "shadow /= totalShadow;\n";
    }
}

}
