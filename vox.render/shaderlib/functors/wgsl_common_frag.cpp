//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_common_frag.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLCommonFrag::WGSLCommonFrag(const std::string& inputStructName) :
_inputStructName(inputStructName) {
    _cameraStruct = "struct CameraData {\n";
    _cameraStruct += " u_viewMat: mat4x4<f32>;\n";
    _cameraStruct += " u_projMat: mat4x4<f32>;\n";
    _cameraStruct += " u_VPMat: mat4x4<f32>;\n";
    _cameraStruct += " u_viewInvMat: mat4x4<f32>;\n";
    _cameraStruct += " u_projInvMat: mat4x4<f32>;\n";
    _cameraStruct += " u_cameraPos: vec3<f32>;\n";
    _cameraStruct += "}\n";
    
    _rendererStruct = "struct RendererData {\n";
    _rendererStruct += " u_localMat: mat4x4<f32>;\n";
    _rendererStruct += " u_modelMat: mat4x4<f32>;\n";
    _rendererStruct += " u_MVMat: mat4x4<f32>;\n";
    _rendererStruct += " u_MVPMat: mat4x4<f32>;\n";
    _rendererStruct += " u_MVInvMat: mat4x4<f32>;\n";
    _rendererStruct += " u_normalMat: mat4x4<f32>;\n";
    _rendererStruct += "}\n";
}

void WGSLCommonFrag::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct(_cameraStruct);
    encoder.addUniformBinding("u_cameraData", "CameraData");
    encoder.addStruct(_rendererStruct);
    encoder.addUniformBinding("u_rendererData", "RendererData");
}

}
