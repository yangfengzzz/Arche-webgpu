//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_direct_irradiance_frag_define.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLDirectIrradianceFragDefine::WGSLDirectIrradianceFragDefine(const std::string& outputStructName) :
_outputStructName(outputStructName) {
    _paramName = "in";
}

void WGSLDirectIrradianceFragDefine::setParamName(const std::string& name) {
    _paramName = name;
}

const std::string& WGSLDirectIrradianceFragDefine::paramName() const {
    return _paramName;
}

void WGSLDirectIrradianceFragDefine::operator()(WGSLEncoder& encoder,
                                                const ShaderMacroCollection& macros, size_t counterIndex) {
    std::string function;
    function = "fn addDirectRadiance(incidentDirection:vec3<f32>, color:vec3<f32>, geometry:GeometricContext, \n";
    function += "material:PhysicalMaterial, reflectedLight:ptr<function, ReflectedLight>) {\n";
    function += "   var dotNL = saturate( dot( geometry.normal, incidentDirection ) );\n";
    function += "\n";
    function += "   var irradiance = dotNL * color;\n";
    function += "   irradiance = irradiance * PI;\n";
    function += "\n";
    function += "   (*reflectedLight).directSpecular = (*reflectedLight).directSpecular + irradiance * BRDF_Specular_GGX( incidentDirection, geometry, material.specularColor, material.roughness);\n";
    function += "\n";
    function += "   (*reflectedLight).directDiffuse = (*reflectedLight).directDiffuse + irradiance * BRDF_Diffuse_Lambert( material.diffuseColor );\n";
    function += "}\n";
    encoder.addFunction(function);
    
    if (macros.contains(DIRECT_LIGHT_COUNT)) {
        function = "fn addDirectionalDirectLightRadiance(directionalLight:DirectLight, geometry:GeometricContext, material:PhysicalMaterial, reflectedLight:ptr<function, ReflectedLight>) {\n";
        function += "    var color = directionalLight.color;\n";
        function += "    var direction = -directionalLight.direction;\n";
        function += "\n";
        function += "   addDirectRadiance( direction, color, geometry, material, reflectedLight );\n";
        function += "}\n";
        encoder.addFunction(function);
    }
    if (macros.contains(POINT_LIGHT_COUNT)) {
        function = "void addPointDirectLightRadiance(pointLight:PointLight, geometry:GeometricContext, material:PhysicalMaterial, reflectedLight:ptr<function, ReflectedLight>) {\n";
        function += "    var lVector = pointLight.position - geometry.position;\n";
        function += "    var direction = normalize( lVector );\n";
        function += "\n";
        function += "    var lightDistance = length( lVector );\n";
        function += "\n";
        function += "    var color = pointLight.color;\n";
        function += "    color = color * clamp(1.0 - pow(lightDistance/pointLight.distance, 4.0), 0.0, 1.0);\n";
        function += "\n";
        function += "    addDirectRadiance( direction, color, geometry, material, reflectedLight );\n";
        function += "}\n";
        encoder.addFunction(function);
    }
    if (macros.contains(SPOT_LIGHT_COUNT)) {
        function = "fn addSpotDirectLightRadiance(spotLight:SpotLight, geometry:GeometricContext, material:PhysicalMaterial, reflectedLight:ptr<function, ReflectedLight>) {\n";
        function += "    var lVector = spotLight.position - geometry.position;\n";
        function += "    var direction = normalize( lVector );\n";
        function += "\n";
        function += "    var lightDistance = length( lVector );\n";
        function += "    var angleCos = dot( direction, -spotLight.direction );\n";
        function += "\n";
        function += "    var spotEffect = smoothstep( spotLight.penumbraCos, spotLight.angleCos, angleCos );\n";
        function += "    var decayEffect = clamp(1.0 - pow(lightDistance/spotLight.distance, 4.0), 0.0, 1.0);\n";
        function += "\n";
        function += "    var color = spotLight.color;\n";
        function += "    color = color * spotEffect * decayEffect;\n";
        function += "\n";
        function += "    addDirectRadiance( direction, color, geometry, material, reflectedLight );\n";
        function += "}\n";
        encoder.addFunction(function);
    }
    
    function = "fn addTotalDirectRadiance(geometry:GeometricContext, material:PhysicalMaterial, reflectedLight:ptr<function, ReflectedLight>){\n";
    if (macros.contains(DIRECT_LIGHT_COUNT)) {
        function += "{\n";
        function += "var i:i32 = 0;\n";
        function += "loop {\n";
        function += fmt::format("if (i >= {}) {{ break; }}\n", (int)*macros.macroConstant(DIRECT_LIGHT_COUNT));
        
        function += "addDirectionalDirectLightRadiance( u_directLight[i], geometry, material, reflectedLight );\n";
        
        function += "i = i + 1;\n";
        function += "}\n";
        function += "}\n";
    }
    if (macros.contains(POINT_LIGHT_COUNT)) {
        function += "{\n";
        function += "var i:i32 = 0;\n";
        function += "loop {\n";
        function += fmt::format("if (i >= {}) {{ break; }}\n", (int)*macros.macroConstant(POINT_LIGHT_COUNT));
        
        function += "addPointDirectLightRadiance( u_pointLight[i], geometry, material, reflectedLight );\n";
        
        function += "i = i + 1;\n";
        function += "}\n";
        function += "}\n";
    }
    if (macros.contains(SPOT_LIGHT_COUNT)) {
        function += "{\n";
        function += "var i:i32 = 0;\n";
        function += "loop {\n";
        function += fmt::format("if (i >= {}) {{ break; }}\n", (int)*macros.macroConstant(SPOT_LIGHT_COUNT));
        
        function += "addSpotDirectLightRadiance( u_spotLight[i], geometry, material, reflectedLight );\n";
        
        function += "i = i + 1;\n";
        function += "}\n";
        function += "}\n";
    }
    function += "}\n";
    encoder.addFunction(function);
}

}
