//
//  wgsl_common.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "wgsl_common.h"

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

}
