//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_common_vert.h"
#include "wgsl.h"
#include <fmt/core.h>

namespace vox {
WGSLCommonVert::WGSLCommonVert(const std::string& inputStructName) :
_inputStructName(inputStructName) {
}

void WGSLCommonVert::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addInoutType(_inputStructName, Attributes::Position, UniformType::Vec3f32);
    if (macros.contains(HAS_UV)) {
        encoder.addInoutType(_inputStructName, Attributes::UV_0, UniformType::Vec2f32);
    }
    
    if (macros.contains(HAS_SKIN)) {
        encoder.addInoutType(_inputStructName, Attributes::Joints_0, UniformType::Vec4f32);
        encoder.addInoutType(_inputStructName, Attributes::Weights_0, UniformType::Vec4f32);
        if (macros.contains(HAS_JOINT_TEXTURE)) {
            // TODO
        } else {
            auto num = macros.macroConstant(JOINTS_COUNT);
            if (num.has_value()) {
                auto formatTemplate = "array<mat4x4<f32>, {}>";
                encoder.addUniformBinding("u_jointMatrix", fmt::format(formatTemplate, (int)*num));
            }
        }
    }
    
    if (macros.contains(HAS_VERTEXCOLOR)) {
        encoder.addInoutType(_inputStructName, Attributes::Color_0, UniformType::Vec4f32);
    }
    
    encoder.addUniformBinding("u_localMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_modelMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_viewMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_projMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_MVMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_MVPMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_normalMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_cameraPos", UniformType::Vec3f32);
    encoder.addUniformBinding("u_tilingOffset", UniformType::Vec4f32);
    
    if (!macros.contains(OMIT_NORMAL)) {
        if (macros.contains(HAS_NORMAL)) {
            encoder.addInoutType(_inputStructName, Attributes::Normal, UniformType::Vec3f32);
        }
        if (macros.contains(HAS_TANGENT)) {
            encoder.addInoutType(_inputStructName, Attributes::Tangent, UniformType::Vec4f32);
        }
    }
}

}
