//
//  wgsl_common.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "wgsl_common.h"

namespace vox {
void WGSLCommon::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    std::string source;
    source += "fn RGBMToLinear(value: vec4<f32>, maxRange: f32): vec4<f32> {\n";
    source += "    return vec4<f32>( value.rgb * value.a * maxRange, 1.0 );\n";
    source += "}\n";
    
    source += "fn gammaToLinear(srgbIn: vec4<f32>): vec4<f32> {\n";
    source += "    return vec4<f32>( pow(srgbIn.rgb, vec3<f32>(2.2)), srgbIn.a);\n";
    source += "}\n";

    source += "fn linearToGamma(linearIn: vec4<f32>): vec4<f32> {\n";
    source += "    return vec4<f32>( pow(linearIn.rgb, vec3<f32>(1.0 / 2.2)), linearIn.a);\n";
    source += "}\n";
    
    encoder.addFunction(source);
}

}
