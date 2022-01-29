//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_common_h
#define wgsl_common_h

#include <string>
#include <vector>

namespace vox {
enum class Attributes : uint32_t {
    Position = 0,
    Normal = 1,
    UV_0 = 2,
    Tangent = 3,
    Bitangent = 4,
    Color_0 = 5,
    Weights_0 = 6,
    Joints_0 = 7,
    UV_1 = 8,
    UV_2 = 9,
    UV_3 = 10,
    UV_4 = 11,
    UV_5 = 12,
    UV_6 = 13,
    UV_7 = 14,
};
std::string attributesToString(Attributes type);

enum class UniformType {
    Vec2f32,
    Vec2i32,
    Vec2u32,
    Vec3f32,
    Vec3i32,
    Vec3u32,
    Vec4f32,
    Vec4i32,
    Vec4u32,
    Mat2x2f32,
    Mat3x2f32,
    Mat4x2f32,
    Mat2x3f32,
    Mat3x3f32,
    Mat4x3f32,
    Mat2x4f32,
    Mat3x4f32,
    Mat4x4f32,
};
std::string uniformTypeToString(UniformType type);

enum class BuiltInType {
    VertexIndex,
    InstanceIndex,
    Position,
    FrontFacing,
    FragDepth,
    LocalInvocationID,
    LocalInvocationIndex,
    GlobalInvocationID,
    WorkgroupID,
    NumWorkgroups,
    SampleIndex,
    SampleMask,
};
std::string builtInTypeToString(BuiltInType type);

}
#endif /* wgsl_common_h */
