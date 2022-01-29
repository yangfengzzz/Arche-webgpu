//
//  blend_shape_input.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "blend_shape_input.h"
#include "wgsl.h"

namespace vox {
WGSLBlendShapeInput::WGSLBlendShapeInput(const std::string& inputStructName) :
_inputStructName(inputStructName) {
}

void WGSLBlendShapeInput::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    if (macros.contains(HAS_BLENDSHAPE)) {
        if (!macros.contains(HAS_BASE_TEXTURE)) {
            encoder.addInoutType(_inputStructName, 10, "POSITION_BS0", UniformType::Vec3f32);
            encoder.addInoutType(_inputStructName, 10, "POSITION_BS1", UniformType::Vec3f32);
            encoder.addInoutType(_inputStructName, 10, "POSITION_BS2", UniformType::Vec3f32);
            encoder.addInoutType(_inputStructName, 10, "POSITION_BS3", UniformType::Vec3f32);

            if (macros.contains(HAS_BLENDSHAPE_NORMAL)) {
                encoder.addInoutType(_inputStructName, 10, "NORMAL_BS0", UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, 10, "NORMAL_BS1", UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, 10, "NORMAL_BS2", UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, 10, "NORMAL_BS3", UniformType::Vec3f32);
            }
            
            if (macros.contains(HAS_BLENDSHAPE_TANGENT)) {
                encoder.addInoutType(_inputStructName, 10, "TANGENT_BS0", UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, 10, "TANGENT_BS1", UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, 10, "TANGENT_BS2", UniformType::Vec3f32);
                encoder.addInoutType(_inputStructName, 10, "TANGENT_BS3", UniformType::Vec3f32);
            }
        }
        encoder.addUniformBinding("u_blendShapeWeights", "array<f32, 4>");
    }
}

}
