//
//  wgsl_library.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/28.
//

#include "wgsl_library.h"
#include "shader/shader.h"
#include <fmt/core.h>

namespace vox {
std::string uniformTypeToString(UniformType type) {
    switch (type) {
        case UniformType::Vec2f32:
            return "vec2<f32>";
            break;
            
        case UniformType::Vec2i32:
            return "vec2<i32>";
            break;
            
        case UniformType::Vec2u32:
            return "vec2<u32>";
            break;
            
        case UniformType::Vec3f32:
            return "vec3<f32>";
            break;
            
        case UniformType::Vec3i32:
            return "vec3<i32>";
            break;
            
        case UniformType::Vec3u32:
            return "vec3<u32>";
            break;
            
        case UniformType::Vec4f32:
            return "vec4<f32>";
            break;
            
        case UniformType::Vec4i32:
            return "vec4<i32>";
            break;
            
        case UniformType::Vec4u32:
            return "vec4<u32>";
            break;
            
        case UniformType::Mat2x2f32:
            return "mat2x2<f32>";
            break;
            
        case UniformType::Mat3x2f32:
            return "mat3x2<f32>";
            break;
            
        case UniformType::Mat4x2f32:
            return "mat4x2<f32>";
            break;
            
        case UniformType::Mat2x3f32:
            return "mat2x3<f32>";
            break;
            
        case UniformType::Mat3x3f32:
            return "mat3x3<f32>";
            break;
            
        case UniformType::Mat4x3f32:
            return "mat4x3<f32>";
            break;
            
        case UniformType::Mat2x4f32:
            return "mat2x4<f32>";
            break;
            
        case UniformType::Mat3x4f32:
            return "mat3x4<f32>";
            break;
            
        case UniformType::Mat4x4f32:
            return "mat4x4<f32>";
            break;
            
        default:
            break;
    }
}

//MARK: - WGSLUniformBinding
WGSLUniformBinding::WGSLUniformBinding(WGSL* source, const std::string& name,
                                       UniformType type, uint32_t group):
_source(source),
_name(name),
_type(type),
_group(group) {
}

void WGSLUniformBinding::operator()() {
    _source->addUniformBinding(_name, _type, _group);
}

//MARK: -
WGSLPatchTest::WGSLPatchTest(WGSL* source):
_source(source),
_uPMatirx(source, "u_projMat", UniformType::Mat4x4f32),
_uMVMatrix(source, "u_MVMat", UniformType::Mat4x4f32) {
}

void WGSLPatchTest::operator()(const ShaderMacroCollection& macros) {
    _source->begin(wgpu::ShaderStage::Vertex);
    _uPMatirx();
    _uMVMatrix();
    
    _source->beginInputStruct("VertexInput");
    _source->addInputType("@location(0) aVertexPosition: vec3<f32>;");
    _source->addInputType("@location(1) aVertexNormal: vec3<f32>;");
    _source->addInputType("@location(2) aVertexUV: vec2<f32>;\n");
    _source->endInputStruct();
    
    _source->beginOutputStruct("Output");
    _source->addOutputType("@location(0) vColor: vec3<f32>;");
    _source->addOutputType("@builtin(position) Position: vec4<f32>;");
    _source->endOutputStruct();
    
    _source->beginEntry({"vertexInput"});
    _source->addEntry("var output: Output;\n "
                      "output.Position = u_projMat * u_MVMat * vec4<f32>(vertexInput.aVertexPosition, 1.0);\n "
                      "output.vColor = vertexInput.aVertexPosition;\n "
                      "return output;\n ");
    _source->endEntry();
}


}
