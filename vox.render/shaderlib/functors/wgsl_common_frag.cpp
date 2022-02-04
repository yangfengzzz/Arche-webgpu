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
}

void WGSLCommonFrag::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addUniformBinding("u_localMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_modelMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_viewMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_projMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_MVMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_MVPMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_normalMat", UniformType::Mat3x3f32);
    encoder.addUniformBinding("u_cameraPos", UniformType::Vec3f32);
}

}
