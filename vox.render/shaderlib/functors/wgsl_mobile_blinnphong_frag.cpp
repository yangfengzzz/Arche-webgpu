//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_mobile_blinnphong_frag.h"
#include <fmt/core.h>

namespace vox {
WGSLMobileBlinnphongFrag::WGSLMobileBlinnphongFrag(const std::string& input, const std::string& output):
_input(input),
_output(output) {
}

void WGSLMobileBlinnphongFrag::operator()(std::string& source, const ShaderMacroCollection& macros) {
    source += fmt::format("var N = getNormal({});\n", _input);
    source += "var lightDiffuse = vec3<f32>( 0.0, 0.0, 0.0 );\n";
    source += "var lightSpecular = vec3<f32>( 0.0, 0.0, 0.0 );\n";
    
    if (macros.contains(DIRECT_LIGHT_COUNT)) {
        source += fmt::format("for( var i:i32 = 0; i < {}; i++ ) {{\n", (int)*macros.macroConstant(DIRECT_LIGHT_COUNT));
        source += "    var d:f32 = max(dot(N, -u_directLight[i].direction), 0.0);\n";
        source += "    lightDiffuse += u_directLight[i].color * d;\n";
        source += "\n";
        source += "    var halfDir:vec3<f32> = normalize( V - u_directLight[i].direction );\n";
        source += "    var s:f32 = pow( clamp( dot( N, halfDir ), 0.0, 1.0 ), u_shininess );\n";
        source += "    lightSpecular += u_directLight[i].color * s;\n";
        source += "}\n";
    }
    
    if (macros.contains(POINT_LIGHT_COUNT)) {
        source += fmt::format("for( var i:i32 = 0; i < {}; i++ ) {{\n", (int)*macros.macroConstant(POINT_LIGHT_COUNT));
        source += fmt::format("    var direction = {}.v_pos - u_pointLight[i].position;\n", _input);
        source += "    var dist = length( direction );\n";
        source += "    direction /= dist;\n";
        source += "    var decay = clamp(1.0 - pow(dist / u_pointLight[i].distance, 4.0), 0.0, 1.0);\n";
        source += "\n";
        source += "    var d =  max( dot( N, -direction ), 0.0 ) * decay;\n";
        source += "    lightDiffuse += u_pointLight[i].color * d;\n";
        source += "\n";
        source += "    var halfDir = normalize( V - direction );\n";
        source += "    var s = pow( clamp( dot( N, halfDir ), 0.0, 1.0 ), u_shininess )  * decay;\n";
        source += "    lightSpecular += u_pointLight[i].color * s;\n";
        source += "}\n";
    }
    
    if (macros.contains(SPOT_LIGHT_COUNT)) {
        source += fmt::format("for( var i:i32 = 0; i < {}; i++) {{\n", (int)*macros.macroConstant(SPOT_LIGHT_COUNT));
        source += fmt::format("    var direction = spotLight.position - {}.v_pos;\n", _input);
        source += "    var lightDistance = length( direction );\n";
        source += "    direction/ = lightDistance;\n";
        source += "    var angleCos = dot( direction, -spotLight.direction );\n";
        source += "    var decay = clamp(1.0 - pow(lightDistance/spotLight.distance, 4.0), 0.0, 1.0);\n";
        source += "    var spotEffect = smoothstep( spotLight.penumbraCos, spotLight.angleCos, angleCos );\n";
        source += "    var decayTotal = decay * spotEffect;\n";
        source += "    var d = max( dot( N, direction ), 0.0 )  * decayTotal;\n";
        source += "    lightDiffuse += spotLight.color * d;\n";
        source += "\n";
        source += "    var halfDir = normalize( V + direction );\n";
        source += "    var s = pow( clamp( dot( N, halfDir ), 0.0, 1.0 ), u_shininess ) * decayTotal;\n";
        source += "    lightSpecular += spotLight.color * s;\n";
        source += "}\n";
    }
    
    source += "diffuse = diffuse * vec4<f32>(lightDiffuse, 1.0);\n";
    source += "specular = specular * vec4<f32>(lightSpecular, 1.0);\n";
    if (macros.contains(NEED_ALPHA_CUTOFF)) {
        source += "if( diffuse.a < u_alphaCutoff ) {\n";
        source += "    discard;\n";
        source += "}\n";
    }
}

}
