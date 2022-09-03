//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_pbr_helper.h"

#include <spdlog/fmt/fmt.h>

#include "vox.render/shaderlib/wgsl.h"

namespace vox {
WGSLPbrHelper::WGSLPbrHelper(const std::string& outputStructName, bool is_metallic_workflow)
    : _outputStructName(outputStructName),
      _is_metallic_workflow(is_metallic_workflow),
      _normalGet(outputStructName),
      _brdf(outputStructName),
      _directIrradianceFragDefine(outputStructName),
      _iblFragDefine(outputStructName) {
    _paramName = "in";
}

void WGSLPbrHelper::setParamName(const std::string& name) { _paramName = name; }

const std::string& WGSLPbrHelper::paramName() const { return _paramName; }

void WGSLPbrHelper::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros, size_t counterIndex) {
    _normalGet(encoder, macros, counterIndex);

    encoder.addFunction(
            "fn pow2(x: f32)->f32 {\n"
            "    return x * x;\n"
            "}\n");
    encoder.addFunction(
            "fn BRDF_Diffuse_Lambert(diffuseColor: vec3<f32>)->vec3<f32> {\n"
            "    return RECIPROCAL_PI * diffuseColor;\n"
            "}\n");
    encoder.addFunction(
            "fn computeSpecularOcclusion(ambientOcclusion: f32, roughness: f32, dotNV: f32)->f32 {\n"
            "    return saturate( pow( dotNV + ambientOcclusion, exp2( - 16.0 * roughness - 1.0 ) ) - 1.0 + "
            "ambientOcclusion );\n"
            "}\n");

    std::string getPhysicalMaterial = "fn getPhysicalMaterial(\n";
    getPhysicalMaterial += "     diffuseColor: vec4<f32>,\n";
    if (_is_metallic_workflow) {
        getPhysicalMaterial += "     metal: f32,\n";
        getPhysicalMaterial += "     roughness: f32,\n";
    } else {
        getPhysicalMaterial += "     specularColor: vec3<f32>,\n";
        getPhysicalMaterial += "     glossiness: f32,\n";
    }
    getPhysicalMaterial += "     alphaCutoff: f32,\n";
    if (macros.contains(HAS_BASE_COLORMAP)) {
        getPhysicalMaterial += "     v_uv: vec2<f32>,\n";
        getPhysicalMaterial += "     u_baseColorTexture: texture_2d<f32>,\n";
        getPhysicalMaterial += "     u_baseColorSampler: sampler,\n";
    }
    if (macros.contains(HAS_VERTEXCOLOR)) {
        getPhysicalMaterial += "     v_color: vec4<f32>,\n";
    }
    if (macros.contains(HAS_METALROUGHNESSMAP) && _is_metallic_workflow) {
        getPhysicalMaterial += "     u_metallicRoughnessTexture: texture_2d<f32>,\n";
        getPhysicalMaterial += "     u_metallicRoughnessSampler: sampler,\n";
    }
    if (macros.contains(HAS_SPECULARGLOSSINESSMAP) && !_is_metallic_workflow) {
        getPhysicalMaterial += "     u_specularGlossinessTexture: texture_2d<f32>,\n";
        getPhysicalMaterial += "     u_specularGlossinessSampler: sampler,\n";
    }
    getPhysicalMaterial += "    )-> PhysicalMaterial {\n";
    getPhysicalMaterial += "        var material: PhysicalMaterial;\n";
    getPhysicalMaterial += "        var diffuseColorUpdate = diffuseColor;\n";
    if (macros.contains(HAS_BASE_COLORMAP)) {
        getPhysicalMaterial += "var baseColor = textureSample(u_baseColorTexture, u_baseColorSampler, v_uv);\n";
        getPhysicalMaterial += "diffuseColorUpdate = diffuseColorUpdate * baseColor;\n";
    }
    if (macros.contains(HAS_VERTEXCOLOR)) {
        getPhysicalMaterial += "diffuseColorUpdate = diffuseColorUpdate * v_color;\n";
    }
    if (macros.contains(NEED_ALPHA_CUTOFF)) {
        getPhysicalMaterial += "if( diffuseColorUpdate.a < alphaCutoff ) {\n";
        getPhysicalMaterial += "    discard;\n";
        getPhysicalMaterial += "}\n";
    }
    if (macros.contains(HAS_METALROUGHNESSMAP) && _is_metallic_workflow) {
        getPhysicalMaterial +=
                "var metalRoughMapColor = textureSample(u_metallicRoughnessTexture, u_metallicRoughnessSampler, v_uv "
                ");\n";
        getPhysicalMaterial += "roughness = roughness * metalRoughMapColor.g;\n";
        getPhysicalMaterial += "metal = metal * metalRoughMapColor.b;\n";
    }
    if (macros.contains(HAS_SPECULARGLOSSINESSMAP) && !_is_metallic_workflow) {
        getPhysicalMaterial +=
                "var specularGlossinessColor = textureSample(u_specularGlossinessTexture, u_specularGlossinessSampler, "
                "v_uv );\n";
        getPhysicalMaterial += "specularColor = specularColor * specularGlossinessColor.rgb;\n";
        getPhysicalMaterial += "glossiness = glossiness * specularGlossinessColor.a;\n";
    }

    if (_is_metallic_workflow) {
        getPhysicalMaterial += "material.diffuseColor = diffuseColorUpdate.rgb * ( 1.0 - metal );\n";
        getPhysicalMaterial += "material.specularColor = mix( vec3<f32>( 0.04), diffuseColorUpdate.rgb, metal );\n";
        getPhysicalMaterial += "material.roughness = clamp( roughness, 0.04, 1.0 );\n";
    } else {
        getPhysicalMaterial +=
                "var specularStrength = max( max( specularColor.r, specularColor.g ), specularColor.b );\n";
        getPhysicalMaterial += "material.diffuseColor = diffuseColorUpdate.rgb * ( 1.0 - specularStrength );\n";
        getPhysicalMaterial += "material.specularColor = specularColor;\n";
        getPhysicalMaterial += "material.roughness = clamp( 1.0 - glossiness, 0.04, 1.0 );\n";
    }

    getPhysicalMaterial += "material.opacity = diffuseColorUpdate.a;\n";
    getPhysicalMaterial += "return material;\n";
    getPhysicalMaterial += "}\n";
    encoder.addFunction(getPhysicalMaterial);

    _brdf(encoder, macros, counterIndex);
    _directIrradianceFragDefine(encoder, macros, counterIndex);
    _iblFragDefine(encoder, macros, counterIndex);
}

}  // namespace vox
