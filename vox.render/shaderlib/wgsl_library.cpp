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
            return "Mat2x2<f32>";
            break;
            
        case UniformType::Mat3x2f32:
            return "Mat3x2<f32>";
            break;
            
        case UniformType::Mat4x2f32:
            return "Mat4x2<f32>";
            break;
            
        case UniformType::Mat2x3f32:
            return "Mat2x3<f32>";
            break;
            
        case UniformType::Mat3x3f32:
            return "Mat3x3<f32>";
            break;
            
        case UniformType::Mat4x3f32:
            return "Mat4x3<f32>";
            break;
            
        case UniformType::Mat2x4f32:
            return "Mat2x4<f32>";
            break;
            
        case UniformType::Mat3x4f32:
            return "Mat3x4<f32>";
            break;
            
        case UniformType::Mat4x4f32:
            return "Mat4x4<f32>";
            break;
            
        default:
            break;
    }
}

//MARK: - WGSLUniformBinding
WGSLUniformBinding::WGSLUniformBinding(const std::string& name, UniformType type, uint32_t group):
_name(name),
_type(type),
_group(group) {
    auto property = Shader::getPropertyByName(name);
    if (property.has_value()) {
        _binding = property.value().uniqueId;
    } else {
        assert(false);
    }
}

std::string WGSLUniformBinding::operator()() {
    const std::string formatTemplate = "@group({}) @binding({})\n "
    "var<uniform> {}: {};\n ";
    
    return fmt::format(formatTemplate, _group, _binding, _name, uniformTypeToString(_type));
}

//MARK: -
WGSLPatchTest::WGSLPatchTest(std::vector<std::string>& uniform,
                             std::vector<std::string>& input,
                             std::vector<std::string>& output,
                             std::vector<std::string>& entry,
                             std::vector<std::string>& function):
_uniform(uniform),
_input(input),
_output(output),
_entry(entry),
_function(function),
_uPMatirx("u_projMat", UniformType::Mat4x4f32),
_uMVMatrix("u_MVMat", UniformType::Mat4x4f32) {
}

void WGSLPatchTest::operator()() {
    _uniform.push_back(_uPMatirx());
    _uniform.push_back(_uMVMatrix());
    
    _input.push_back("@location(0) aVertexPosition: vec3<f32>;\n "
                     "@location(1) aVertexNormal: vec3<f32>;\n "
                     "@location(2) aVertexUV: vec2<f32>;\n ");
    _output.push_back("@location(0) vColor: vec3<f32>;\n "
                      "@builtin(position) Position: vec4<f32>;\n ");
    _entry.push_back("var output: Output;\n"
                     "output.Position = uPMatrix * uMVMatrix * vec4<f32>(vertexInput.aVertexPosition, 1.0);\n"
                     "output.vColor = vertexInput.aVertexPosition;\n"
                     "return output;\n");
}


}
