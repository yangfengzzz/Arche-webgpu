//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_shadow_frag.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLShadowFrag::WGSLShadowFrag(){
}

void WGSLShadowFrag::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    if (macros.contains(SHADOW_MAP_COUNT)) {
        encoder.addUniformBinding("u_shadowData", "ShadowData");
        encoder.addSampledTextureBinding("u_shadowMap", TextureType::TextureDepth2DArray, "u_shadowSampler", SamplerType::SamplerComparison);
    }
    
    if (macros.contains(CUBE_SHADOW_MAP_COUNT)) {
        encoder.addUniformBinding("u_cubeShadowData", "CubeShadowData");
        encoder.addSampledTextureBinding("u_cubeShadowMap", TextureType::TextureDepthCubeArray, "u_cubeShadowSampler", SamplerType::SamplerComparison);
    }
}

void WGSLShadowFrag::operator()(std::string& source, const ShaderMacroCollection& macros) {
    source += "var shadow:f32 = 0.0;\n";
    source += "var totalShadow:f32 = 0.0;\n";
    
    if (macros.contains(SHADOW_MAP_COUNT)) {
        source += "{\n";
        source += "var i:i32 = 0;\n";
        source += "loop {\n";
        source += fmt::format("if (i >= {}) {{ break; }}\n", (int)*macros.macroConstant(SHADOW_MAP_COUNT));
        
        source += "shadow = shadow + filterPCF(in.v_pos, in.view_pos, u_shadowData, i, u_shadowMap, u_shadowSampler);\n";
        // source += "shadow = shadow + textureProj(in.v_pos, in.view_pos, float2(0), u_shadowData, i, u_shadowMap, u_shadowSampler);\n";

        source += "i = i + 1;\n";
        source += "}\n";
        source += "}\n";
        
        source += fmt::format("totalShadow = totalShadow + {}.0;\n", (int)*macros.macroConstant(SHADOW_MAP_COUNT));
    }
    
    if (macros.contains(CUBE_SHADOW_MAP_COUNT)) {
        source += "{\n";
        source += "var i:i32 = 0;\n";
        source += "loop {\n";
        source += fmt::format("if (i >= {}) {{ break; }}\n", (int)*macros.macroConstant(CUBE_SHADOW_MAP_COUNT));
        
        // source += "shadow = shadow + cubeFilterPCF(in.v_pos, u_cubeShadowData, i, u_cubeShadowMap, u_cubeShadowSampler);\n"; // too expensive
        source += "shadow = shadow + cubeTextureProj(in.v_pos, vec2<f32>(0.0, 0.0), u_cubeShadowData, i, u_cubeShadowMap, u_cubeShadowSampler);\n";
        
        source += "i = i + 1;\n";
        source += "}\n";
        source += "}\n";
        
        source += fmt::format("totalShadow = totalShadow + {}.0;\n", (int)*macros.macroConstant(CUBE_SHADOW_MAP_COUNT));
    }
    
    if (macros.contains(SHADOW_MAP_COUNT) || macros.contains(CUBE_SHADOW_MAP_COUNT)) {
        source += "shadow = shadow / totalShadow;\n";
    }
}

}
