//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_brdf.h"

#include <spdlog/fmt/fmt.h>

#include "vox.render/shaderlib/wgsl.h"

namespace vox {
WGSLBRDF::WGSLBRDF(const std::string& outputStructName) : _outputStructName(outputStructName) { _paramName = "in"; }

void WGSLBRDF::setParamName(const std::string& name) { _paramName = name; }

const std::string& WGSLBRDF::paramName() const { return _paramName; }

void WGSLBRDF::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros, size_t counterIndex) {
    std::string function;

    // Original approximation by Christophe Schlick '94
    // float fresnel = pow( 1.0 - dotLH, 5.0 );
    // Optimized variant (presented by Epic at SIGGRAPH '13)
    // https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
    function = "fn F_Schlick(specularColor: vec3<f32>, dotLH: f32)->vec3<f32> {\n";
    function += "   var fresnel = exp2( ( -5.55473 * dotLH - 6.98316 ) * dotLH );\n";
    function += "   return ( 1.0 - specularColor ) * fresnel + specularColor;\n";
    function += "}\n";
    encoder.addFunction(function);

    // Moving Frostbite to Physically Based Rendering 3.0 - page 12, listing 2
    // https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf
    function = "fn G_GGX_SmithCorrelated(alpha: f32, dotNL: f32, dotNV: f32)->f32 {\n";
    function += "    var a2 = pow2( alpha );\n";
    function += "\n";
    function += "    // dotNL and dotNV are explicitly swapped. This is not a mistake.\n";
    function += "    var gv = dotNL * sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNV ) );\n";
    function += "    var gl = dotNV * sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNL ) );\n";
    function += "\n";
    function += "    return 0.5 / max( gv + gl, EPSILON );\n";
    function += "}\n";
    encoder.addFunction(function);

    // Microfacet Models for Refraction through Rough Surfaces - equation (33)
    // http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
    // alpha is "roughness squared" in Disney’s reparameterization
    function = "fn D_GGX(alpha: f32, dotNH: f32)->f32 {\n";
    function += "   var a2 = pow2( alpha );\n";
    function += "\n";
    function += "    var denom = pow2( dotNH ) * ( a2 - 1.0 ) + 1.0; // avoid alpha = 0 with dotNH = 1\n";
    function += "\n";
    function += "   return RECIPROCAL_PI * a2 / pow2( denom );\n";
    function += "}\n";
    encoder.addFunction(function);

    // GGX Distribution, Schlick Fresnel, GGX-Smith Visibility
    function =
            "fn BRDF_Specular_GGX(incidentDirection:vec3<f32>, geometry:GeometricContext, specularColor:vec3<f32>, "
            "roughness:f32)->vec3<f32> {\n";
    function += "\n";
    function += "    var alpha = pow2( roughness ); // UE4's roughness\n";
    function += "\n";
    function += "    var halfDir = normalize( incidentDirection + geometry.viewDir );\n";
    function += "\n";
    function += "    var dotNL = saturate( dot( geometry.normal, incidentDirection ) );\n";
    function += "    var dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );\n";
    function += "    var dotNH = saturate( dot( geometry.normal, halfDir ) );\n";
    function += "   var dotLH = saturate( dot( incidentDirection, halfDir ) );\n";
    function += "\n";
    function += "    var F = F_Schlick( specularColor, dotLH );\n";
    function += "\n";
    function += "   var G = G_GGX_SmithCorrelated( alpha, dotNL, dotNV );\n";
    function += "\n";
    function += "   var D = D_GGX( alpha, dotNH );\n";
    function += "\n";
    function += "   return F * ( G * D );\n";
    function += "\n";
    function += "}\n";
    encoder.addFunction(function);
}

}  // namespace vox
