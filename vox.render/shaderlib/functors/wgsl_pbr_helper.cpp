//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_pbr_helper.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLPbrHelper::WGSLPbrHelper(const std::string& outputStructName, bool is_metallic_workflow) :
_outputStructName(outputStructName),
_is_metallic_workflow(is_metallic_workflow),
_normalGet(outputStructName),
_brdf(outputStructName),
_directIrradianceFragDefine(outputStructName),
_iblFragDefine(outputStructName){
    _paramName = "in";
}

void WGSLPbrHelper::setParamName(const std::string& name) {
    _paramName = name;
}

const std::string& WGSLPbrHelper::paramName() const {
    return _paramName;
}

void WGSLPbrHelper::operator()(WGSLEncoder& encoder,
                               const ShaderMacroCollection& macros, size_t counterIndex) {
    _normalGet(encoder, macros, counterIndex);
    
    encoder.addFunction("fn pow2(x: f32)->f32 {\n"
                        "    return x * x;\n"
                        "}\n");
    encoder.addFunction("fn BRDF_Diffuse_Lambert(diffuseColor: vec3<f32>)->vec3<f32> {\n"
                        "    return RECIPROCAL_PI * diffuseColor;\n"
                        "}\n");
    encoder.addFunction("fn computeSpecularOcclusion(ambientOcclusion: f32, roughness: f32, dotNV: f32)->f32 {\n"
                        "    return saturate( pow( dotNV + ambientOcclusion, exp2( - 16.0 * roughness - 1.0 ) ) - 1.0 + ambientOcclusion );\n"
                        "}\n");
    
    
    std::string getPhysicalMaterial = "fn getPhysicalMaterial(\n";
    getPhysicalMaterial += "     diffuseColor: vec4<f32>,\n";
    getPhysicalMaterial += "     metal: f32,\n";
    getPhysicalMaterial += "     roughness: f32,\n";
    getPhysicalMaterial += "     specularColor: vec3<f32>,\n";
    getPhysicalMaterial += "     glossiness: f32,\n";
    getPhysicalMaterial += "     alphaCutoff: f32\n";
    getPhysicalMaterial += "    )-> PhysicalMaterial {\n";
    getPhysicalMaterial += "        var material: PhysicalMaterial;\n";
    if (macros.contains(HAS_BASE_COLORMAP)) {
        getPhysicalMaterial += fmt::format("var baseColor = textureSample(u_baseColorTexture, u_baseColorSampler, {}.v_uv);\n", _paramName);
        getPhysicalMaterial += "diffuseColor = diffuseColor * baseColor;\n";
    }
    if (macros.contains(HAS_VERTEXCOLOR)) {
        getPhysicalMaterial += "diffuseColor = diffuseColor * v_color;\n";
    }
    if (macros.contains(NEED_ALPHA_CUTOFF)) {
        getPhysicalMaterial += "if( diffuseColor.a < alphaCutoff ) {\n";
        getPhysicalMaterial += "    discard;\n";
        getPhysicalMaterial += "}\n";
    }
    if (macros.contains(HAS_METALROUGHNESSMAP) && _is_metallic_workflow) {
        getPhysicalMaterial += fmt::format("var metalRoughMapColor = textureSample(u_metallicRoughnessTexture, u_metallicRoughnessSampler, {}.v_uv );\n", _paramName);
        getPhysicalMaterial += "roughness = roughness * metalRoughMapColor.g;\n";
        getPhysicalMaterial += "metal = metal * metalRoughMapColor.b;\n";
    }
    if (macros.contains(HAS_SPECULARGLOSSINESSMAP) && !_is_metallic_workflow) {
        getPhysicalMaterial += fmt::format("var specularGlossinessColor = textureSample(u_specularGlossinessTexture, u_specularGlossinessSampler, {}.v_uv );\n", _paramName);
        getPhysicalMaterial += "specularColor *= specularGlossinessColor.rgb;\n";
        getPhysicalMaterial += "glossiness *= specularGlossinessColor.a;\n";
    }
    
    if (_is_metallic_workflow) {
        getPhysicalMaterial += "material.diffuseColor = diffuseColor.rgb * ( 1.0 - metal );\n";
        getPhysicalMaterial += "material.specularColor = mix( vec3<f32>( 0.04), diffuseColor.rgb, metal );\n";
        getPhysicalMaterial += "material.roughness = clamp( roughness, 0.04, 1.0 );\n";
    } else {
        getPhysicalMaterial += "var specularStrength = max( max( specularColor.r, specularColor.g ), specularColor.b );\n";
        getPhysicalMaterial += "material.diffuseColor = diffuseColor.rgb * ( 1.0 - specularStrength );\n";
        getPhysicalMaterial += "material.specularColor = specularColor;\n";
        getPhysicalMaterial += "material.roughness = clamp( 1.0 - glossiness, 0.04, 1.0 );\n";
    }
    
    getPhysicalMaterial += "material.opacity = diffuseColor.a;\n";
    getPhysicalMaterial += "return material;\n";
    getPhysicalMaterial += "}\n";
    encoder.addFunction(getPhysicalMaterial);
    
    _brdf(encoder, macros, counterIndex);
    _directIrradianceFragDefine(encoder, macros, counterIndex);
    _iblFragDefine(encoder, macros, counterIndex);
}

}
