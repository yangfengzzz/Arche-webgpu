//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_common.h"

namespace vox {
void WGSLCommon::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct("let PI:f32 = 3.14159265359;\n");
    encoder.addStruct("let RECIPROCAL_PI:f32 = 0.31830988618;\n");
    encoder.addStruct("let EPSILON:f32 = 1.0e-6;\n");
    encoder.addStruct("let LOG2:f32 = 1.442695;\n");
    
    std::string source;
    source += "fn saturate(a:f32)->f32 { return clamp( a, 0.0, 1.0 );}\n";
    source += "fn whiteCompliment(a:f32)->f32 { return 1.0 - saturate( a );}\n";
    
    source += "fn RGBMToLinear(value: vec4<f32>, maxRange: f32)-> vec4<f32> {\n";
    source += "    return vec4<f32>( value.rgb * value.a * maxRange, 1.0 );\n";
    source += "}\n";
    
    source += "fn gammaToLinear(srgbIn: vec4<f32>)-> vec4<f32> {\n";
    source += "    return vec4<f32>( pow(srgbIn.rgb, vec3<f32>(2.2)), srgbIn.a);\n";
    source += "}\n";

    source += "fn linearToGamma(linearIn: vec4<f32>)-> vec4<f32> {\n";
    source += "    return vec4<f32>( pow(linearIn.rgb, vec3<f32>(1.0 / 2.2)), linearIn.a);\n";
    source += "}\n";
    
    encoder.addFunction(source);
}

}
