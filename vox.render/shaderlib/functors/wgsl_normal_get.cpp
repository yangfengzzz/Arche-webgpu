//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_normal_get.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLNormalGet::WGSLNormalGet(const std::string& outputStructName) :
_outputStructName(outputStructName) {
    _paramName = "in";
}

void WGSLNormalGet::setParamName(const std::string& name) {
    _paramName = name;
}

const std::string& WGSLNormalGet::paramName() const {
    return _paramName;
}

void WGSLNormalGet::operator()(WGSLEncoder& encoder,
                               const ShaderMacroCollection& macros, size_t counterIndex) {
    std::string source = fmt::format("fn getNormal({}:{})->vec3<f32> {{\n", _paramName, _outputStructName);
    if (macros.contains(HAS_NORMAL_TEXTURE)) {
        if (!macros.contains(HAS_TANGENT)) {
            source += fmt::format("var pos_dx = dfdx({}.v_pos);\n", _paramName);
            source += fmt::format("var pos_dy = dfdy({}.v_pos);\n", _paramName);
            source += fmt::format("var tex_dx = dfdx(vec3<f32>({}.v_uv, 0.0));\n", _paramName);
            source += fmt::format("var tex_dy = dfdy(vec3<f32>({}.v_uv, 0.0));\n", _paramName);
            source += "var t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);\n";
            if (macros.contains(HAS_NORMAL)) {
                source += "var ng = normalize(v_normal);\n";
            } else {
                source += "var ng = normalize( cross(pos_dx, pos_dy) );\n";
            }
            source += "t = normalize(t - ng * dot(ng, t));\n";
            source += "var b = normalize(cross(ng, t));\n";
            source += "var tbn = mat3x3<f32>(t, b, ng);\n";
        } else {
            source += fmt::format("var tbn = {}.v_TBN;\n", _paramName);
        }
        source += fmt::format("var n = textureSample(u_normalTexture, u_normalSampler, {}.v_uv ).rgb;\n", _paramName);
        source += "n = normalize(tbn * ((2.0 * n - 1.0) * vec3<f32>(u_normalIntensity, u_normalIntensity, 1.0)));\n";
    } else {
        if (macros.contains(HAS_NORMAL)) {
            source += fmt::format("var n = normalize({}.v_normal);\n", _paramName);
        } else {
            source += fmt::format("var pos_dx = dfdx({}.v_pos);\n", _paramName);
            source += fmt::format("var pos_dy = dfdy({}.v_pos);\n", _paramName);
            source += "var n = normalize( cross(pos_dx, pos_dy) );\n";
        }
    }
//    source += "n = -1.0 * n;\n";
    source += "return n;\n";
    source += "}\n";
    
    encoder.addFunction(source);
}

}
