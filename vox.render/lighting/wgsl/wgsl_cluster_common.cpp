//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_cluster_common.h"

namespace vox {
void WGSLProjectionUniforms::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct("struct ProjectionUniforms {\n"
                      "  matrix : mat4x4<f32>;\n"
                      "  inverseMatrix : mat4x4<f32>;\n"
                      "  outputSize : vec2<f32>;\n"
                      "  zNear : f32;\n"
                      "  zFar : f32;\n"
                      "};\n");
    encoder.addUniformBinding("projection", "ProjectionUniforms");
}

//MARK: - WGSLViewUniforms
void WGSLViewUniforms::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct("struct ViewUniforms {\n"
                      "  matrix : mat4x4<f32>;\n"
                      "  position : vec3<f32>;\n"
                      "};\n");
    encoder.addUniformBinding("view", "ViewUniforms");
}

//MARK: - WGSLLightUniforms
void WGSLLightUniforms::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct("struct Light {\n"
                      "  position : vec3<f32>;\n"
                      "  range : f32;\n"
                      "  color : vec3<f32>;\n"
                      "  intensity : f32;\n"
                      "};\n"
                      "\n"
                      "struct GlobalLightUniforms {\n"
                      "  ambient : vec3<f32>;\n"
                      "  lightCount : u32;\n"
                      "  lights : array<Light>;\n"
                      "};\n");
    encoder.addStorageBufferBinding("globalLights", "GlobalLightUniforms", true);
}

//MARK: - WGSLModelUniforms
void WGSLModelUniforms::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct("struct ModelUniforms {\n"
                      "  matrix : mat4x4<f32>;\n"
                      "};\n");
    encoder.addUniformBinding("model", "ModelUniforms");
}

}
