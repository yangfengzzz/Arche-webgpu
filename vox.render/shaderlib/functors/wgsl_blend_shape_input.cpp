//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_blend_shape_input.h"

#include "vox.render/shaderlib/wgsl.h"

namespace vox {
WGSLBlendShapeInput::WGSLBlendShapeInput(const std::string& inputStructName) : _inputStructName(inputStructName) {}

void WGSLBlendShapeInput::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros, size_t counterIndex) {
    if (macros.contains(HAS_BLENDSHAPE)) {
        if (!macros.contains(HAS_BASE_TEXTURE)) {
            encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "POSITION_BS0",
                                 UniformType::Vec3f32);
            encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "POSITION_BS1",
                                 UniformType::Vec3f32);
            encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "POSITION_BS2",
                                 UniformType::Vec3f32);
            encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "POSITION_BS3",
                                 UniformType::Vec3f32);

            if (macros.contains(HAS_BLENDSHAPE_NORMAL)) {
                encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "NORMAL_BS0",
                                     UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "NORMAL_BS1",
                                     UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "NORMAL_BS2",
                                     UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "NORMAL_BS3",
                                     UniformType::Vec3f32);
            }

            if (macros.contains(HAS_BLENDSHAPE_TANGENT)) {
                encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "TANGENT_BS0",
                                     UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "TANGENT_BS1",
                                     UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "TANGENT_BS2",
                                     UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, WGSLEncoder::getCounterNumber(counterIndex), "TANGENT_BS3",
                                     UniformType::Vec3f32);
            }
        }
        encoder.addUniformBinding("u_blendShapeWeights", "array<f32, 4>");
    }
}

}  // namespace vox
