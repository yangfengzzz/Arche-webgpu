//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_ibl_frag_define.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLIBLFragDefine::WGSLIBLFragDefine(const std::string& outputStructName) :
_outputStructName(outputStructName) {
    _paramName = "in";
}

void WGSLIBLFragDefine::setParamName(const std::string& name) {
    _paramName = name;
}

const std::string& WGSLIBLFragDefine::paramName() const {
    return _paramName;
}

void WGSLIBLFragDefine::operator()(WGSLEncoder& encoder,
                                   const ShaderMacroCollection& macros, size_t counterIndex) {
    std::string function;
    // sh need be pre-scaled in CPU.
    function = "fn getLightProbeIrradiance(sh: array<vec3<f32>, 9>, normal:vec3<f32>)->vec3<f32> {\n";
    function += "      var result = sh[0] +\n";
    function += "\n";
    function += "            sh[1] * (normal.y) +\n";
    function += "            sh[2] * (normal.z) +\n";
    function += "            sh[3] * (normal.x) +\n";
    function += "\n";
    function += "           sh[4] * (normal.y * normal.x) +\n";
    function += "           sh[5] * (normal.y * normal.z) +\n";
    function += "           sh[6] * (3.0 * normal.z * normal.z - 1.0) +\n";
    function += "           sh[7] * (normal.z * normal.x) +\n";
    function += "           sh[8] * (normal.x * normal.x - normal.y * normal.y);\n";
    function += "\n";
    function += "   return max(result, vec3(0.0));\n";
    function += "}\n";
    encoder.addFunction(function);
    
    // ref: https://www.unrealengine.com/blog/physically-based-shading-on-mobile - environmentBRDF for GGX on mobile
    function = "fn envBRDFApprox(specularColor:vec3<f32>, roughness:f32, dotNV:f32 )->vec3<f32>{\n";
    function += "   let c0 = vec4<f32>( - 1, - 0.0275, - 0.572, 0.022 );\n";
    function += "\n";
    function += "   let c1 = vec4<f32>( 1, 0.0425, 1.04, - 0.04 );\n";
    function += "\n";
    function += "   var r = roughness * c0 + c1;\n";
    function += "\n";
    function += "   var a004 = min( r.x * r.x, exp2( - 9.28 * dotNV ) ) * r.x + r.y;\n";
    function += "\n";
    function += "   var AB = vec2<f32>( -1.04, 1.04 ) * a004 + r.zw;\n";
    function += "\n";
    function += "   return specularColor * AB.x + AB.y;\n";
    function += "}\n";
    encoder.addFunction(function);
    
    function = "fn getSpecularMIPLevel(roughness:f32, maxMIPLevel:i32)->f32 {\n";
    function += "    return roughness * float(maxMIPLevel);\n";
    function += "}\n";
    encoder.addFunction(function);
    
    function = "fn getLightProbeRadiance(geometry:GeometricContext, roughness:f32, maxMIPLevel:i32, specularIntensity:f32)->vec3<f32> {\n";
    if (!macros.contains(HAS_SPECULAR_ENV)) {
        function += "return vec3<f32>(0.0, 0.0, 0.0);\n";
    } else {
        function += "var reflectVec = reflect( -geometry.viewDir, geometry.normal );\n";
        function += "var specularMIPLevel = getSpecularMIPLevel(roughness, maxMIPLevel );\n";
        function += "var envMapColor =  textureSample(u_env_specularTexture, u_env_specularSampler, reflectVec, specularMIPLevel );\n";
        function += "return envMapColor.rgb * specularIntensity;\n";
    }
    function += "}\n";
    encoder.addFunction(function);
}

}
