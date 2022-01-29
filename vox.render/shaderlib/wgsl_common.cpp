//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_common.h"

namespace vox {
std::string attributesToString(Attributes type) {
    switch (type) {
        case Attributes::Position:
            return "Position";
            break;
        case Attributes::Normal:
            return "Normal";
            break;
        case Attributes::UV_0:
            return "UV_0";
            break;
        case Attributes::Tangent:
            return "Tangent";
            break;
        case Attributes::Bitangent:
            return "Bitangent";
            break;
        case Attributes::Color_0:
            return "Color_0";
            break;
        case Attributes::Weights_0:
            return "Weights_0";
            break;
        case Attributes::Joints_0:
            return "Joints_0";
            break;
        case Attributes::UV_1:
            return "UV_1";
            break;
        case Attributes::UV_2:
            return "UV_2";
            break;
        case Attributes::UV_3:
            return "UV_3";
            break;
        case Attributes::UV_4:
            return "UV_4";
            break;
        case Attributes::UV_5:
            return "UV_5";
            break;
        case Attributes::UV_6:
            return "UV_6";
            break;
        case Attributes::UV_7:
            return "UV_7";
            break;
        default:
            assert(false && "Unknown attribute");
            break;
    }
}

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

std::string builtInTypeToString(BuiltInType type) {
    switch (type) {
        case BuiltInType::VertexIndex:
            return "vertex_index";
            break;
        case BuiltInType::InstanceIndex:
            return "instance_index";
            break;
        case BuiltInType::Position:
            return "position";
            break;
        case BuiltInType::FrontFacing:
            return "front_facing";
            break;
        case BuiltInType::FragDepth:
            return "frag_depth";
            break;
        case BuiltInType::LocalInvocationID:
            return "local_invocation_id";
            break;
        case BuiltInType::LocalInvocationIndex:
            return "local_invocation_index";
            break;
        case BuiltInType::GlobalInvocationID:
            return "global_invocation_id";
            break;
        case BuiltInType::WorkgroupID:
            return "workgroup_id";
            break;
        case BuiltInType::NumWorkgroups:
            return "num_workgroups";
            break;
        case BuiltInType::SampleIndex:
            return "sample_index";
            break;
        case BuiltInType::SampleMask:
            return "sample_mask";
            break;
        default:
            break;
    }
}

}
