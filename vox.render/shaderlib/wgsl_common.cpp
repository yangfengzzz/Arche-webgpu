//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_common.h"

namespace vox {
// MARK: - Attributes
std::string toString(Attributes type) {
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
            throw std::exception();
            break;
    }
}

// MARK: - UniformType
std::string toString(UniformType type) {
    switch (type) {
        case UniformType::F32:
            return "f32";
            break;

        case UniformType::I32:
            return "i32";
            break;

        case UniformType::U32:
            return "u32";
            break;

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

// MARK: - TextureType
std::string toString(TextureType type) {
    switch (type) {
        case TextureType::Texture1Df32:
            return "texture_1d<f32>";
            break;

        case TextureType::Texture1Di32:
            return "texture_1d<i32>";
            break;

        case TextureType::Texture1Du32:
            return "texture_1d<u32>";
            break;

        case TextureType::Texture2Df32:
            return "texture_2d<f32>";
            break;

        case TextureType::Texture2Di32:
            return "texture_2d<i32>";
            break;

        case TextureType::Texture2Du32:
            return "texture_2d<u32>";
            break;

        case TextureType::Texture2DArrayf32:
            return "texture_2d_array<f32>";
            break;

        case TextureType::Texture2DArrayi32:
            return "texture_2d_array<i32>";
            break;

        case TextureType::Texture2DArrayu32:
            return "texture_2d_array<u32>";
            break;

        case TextureType::Texture3Df32:
            return "texture_3d<f32>";
            break;

        case TextureType::Texture3Di32:
            return "texture_3d<i32>";
            break;

        case TextureType::Texture3Du32:
            return "texture_3d<u32>";
            break;

        case TextureType::TextureCubef32:
            return "texture_cube<f32>";
            break;

        case TextureType::TextureCubei32:
            return "texture_cube<i32>";
            break;

        case TextureType::TextureCubeu32:
            return "texture_cube<u32>";
            break;

        case TextureType::TextureCubeArrayf32:
            return "texture_cube_array<f32>";
            break;

        case TextureType::TextureCubeArrayi32:
            return "texture_cube_array<i32>";
            break;

        case TextureType::TextureCubeArrayu32:
            return "texture_cube_array<u32>";
            break;

        case TextureType::TextureMultisampled2Df32:
            return "texture_multisampled_2d<f32>";
            break;

        case TextureType::TextureMultisampled2Di32:
            return "texture_multisampled_2d<i32>";
            break;

        case TextureType::TextureMultisampled2Du32:
            return "texture_multisampled_2d<u32>";
            break;

        case TextureType::TextureDepth2D:
            return "texture_depth_2d";
            break;

        case TextureType::TextureDepth2DArray:
            return "texture_depth_2d_array";
            break;

        case TextureType::TextureDepthCube:
            return "texture_depth_cube";
            break;

        case TextureType::TextureDepthCubeArray:
            return "texture_depth_cube_array";
            break;

        case TextureType::TextureDepthMultisampled2D:
            return "texture_depth_multisampled_2d";
            break;

        default:
            break;
    }
}

bool isMultisampled(TextureType type) {
    if (type == TextureType::TextureMultisampled2Df32 || type == TextureType::TextureMultisampled2Di32 ||
        type == TextureType::TextureMultisampled2Du32) {
        return true;
    }
    return false;
}

wgpu::TextureViewDimension viewDimension(TextureType type) {
    switch (type) {
        case TextureType::Texture1Df32:
        case TextureType::Texture1Di32:
        case TextureType::Texture1Du32:
            return wgpu::TextureViewDimension::e1D;
            break;

        case TextureType::Texture2Df32:
        case TextureType::Texture2Di32:
        case TextureType::Texture2Du32:
        case TextureType::TextureMultisampled2Df32:
        case TextureType::TextureMultisampled2Di32:
        case TextureType::TextureMultisampled2Du32:
        case TextureType::TextureDepth2D:
        case TextureType::TextureDepthMultisampled2D:
            return wgpu::TextureViewDimension::e2D;
            break;

        case TextureType::Texture2DArrayf32:
        case TextureType::Texture2DArrayi32:
        case TextureType::Texture2DArrayu32:
        case TextureType::TextureDepth2DArray:
            return wgpu::TextureViewDimension::e2DArray;
            break;

        case TextureType::Texture3Df32:
        case TextureType::Texture3Di32:
        case TextureType::Texture3Du32:
            return wgpu::TextureViewDimension::e3D;
            break;

        case TextureType::TextureCubef32:
        case TextureType::TextureCubei32:
        case TextureType::TextureCubeu32:
        case TextureType::TextureDepthCube:
            return wgpu::TextureViewDimension::Cube;
            break;

        case TextureType::TextureCubeArrayf32:
        case TextureType::TextureCubeArrayi32:
        case TextureType::TextureCubeArrayu32:
        case TextureType::TextureDepthCubeArray:
            return wgpu::TextureViewDimension::CubeArray;
            break;

        default:
            break;
    }
}

wgpu::TextureSampleType sampleType(TextureType type) {
    switch (type) {
        case TextureType::Texture1Df32:
        case TextureType::Texture2Df32:
        case TextureType::Texture2DArrayf32:
        case TextureType::Texture3Df32:
        case TextureType::TextureCubef32:
        case TextureType::TextureCubeArrayf32:
        case TextureType::TextureMultisampled2Df32:
            return wgpu::TextureSampleType::Float;
            break;

        case TextureType::Texture1Di32:
        case TextureType::Texture2Di32:
        case TextureType::Texture2DArrayi32:
        case TextureType::Texture3Di32:
        case TextureType::TextureCubei32:
        case TextureType::TextureCubeArrayi32:
        case TextureType::TextureMultisampled2Di32:
            return wgpu::TextureSampleType::Sint;
            break;

        case TextureType::Texture1Du32:
        case TextureType::Texture2Du32:
        case TextureType::Texture2DArrayu32:
        case TextureType::Texture3Du32:
        case TextureType::TextureCubeu32:
        case TextureType::TextureCubeArrayu32:
        case TextureType::TextureMultisampled2Du32:
            return wgpu::TextureSampleType::Uint;
            break;

        case TextureType::TextureDepth2D:
        case TextureType::TextureDepth2DArray:
        case TextureType::TextureDepthCube:
        case TextureType::TextureDepthCubeArray:
        case TextureType::TextureDepthMultisampled2D:
            return wgpu::TextureSampleType::Depth;
            break;

        default:
            break;
    }
}

// MARK: - SamplerType
std::string toString(SamplerType type) {
    switch (type) {
        case SamplerType::Sampler:
            return "sampler";
            break;

        case SamplerType::SamplerComparison:
            return "sampler_comparison";
            break;

        default:
            break;
    }
}

wgpu::SamplerBindingType bindingType(SamplerType type) {
    switch (type) {
        case SamplerType::Sampler:
            return wgpu::SamplerBindingType::Filtering;
            break;

        case SamplerType::SamplerComparison:
            return wgpu::SamplerBindingType::Comparison;
            break;

        default:
            break;
    }
}

// MARK: - StorageTextureType
std::string toString(StorageTextureType type) {
    switch (type) {
        case StorageTextureType::TextureStorage1D:
            return "texture_storage_1d";
            break;
        case StorageTextureType::TextureStorage2D:
            return "texture_storage_2d";
            break;
        case StorageTextureType::TextureStorage2DArray:
            return "texture_storage_2d_array";
            break;
        case StorageTextureType::TextureStorage3D:
            return "texture_storage_3d";
            break;

        default:
            break;
    }
}

wgpu::TextureViewDimension viewDimension(StorageTextureType type) {
    switch (type) {
        case StorageTextureType::TextureStorage1D:
            return wgpu::TextureViewDimension::e1D;
            break;
        case StorageTextureType::TextureStorage2D:
            return wgpu::TextureViewDimension::e2D;
            break;
        case StorageTextureType::TextureStorage2DArray:
            return wgpu::TextureViewDimension::e2DArray;
            break;
        case StorageTextureType::TextureStorage3D:
            return wgpu::TextureViewDimension::e3D;
            break;

        default:
            break;
    }
}

// MARK: - TexelFormat
std::string toString(wgpu::TextureFormat type) {
    switch (type) {
        case wgpu::TextureFormat::RGBA8Unorm:
            return "rgba8unorm";
            break;
        case wgpu::TextureFormat::RGBA8Snorm:
            return "rgba8snorm";
            break;
        case wgpu::TextureFormat::RGBA8Uint:
            return "rgba8uint";
            break;
        case wgpu::TextureFormat::RGBA8Sint:
            return "rgba8sint";
            break;
        case wgpu::TextureFormat::RGBA16Uint:
            return "rgba16uint";
            break;
        case wgpu::TextureFormat::RGBA16Sint:
            return "rgba16sint";
            break;
        case wgpu::TextureFormat::RGBA16Float:
            return "rgba16float";
            break;
        case wgpu::TextureFormat::R32Uint:
            return "r32uint";
            break;
        case wgpu::TextureFormat::R32Sint:
            return "r32sint";
            break;
        case wgpu::TextureFormat::R32Float:
            return "r32float";
            break;
        case wgpu::TextureFormat::RG32Uint:
            return "rg32uint";
            break;
        case wgpu::TextureFormat::RG32Sint:
            return "rg32sint";
            break;
        case wgpu::TextureFormat::RG32Float:
            return "rg32float";
            break;
        case wgpu::TextureFormat::RGBA32Uint:
            return "rgba32uint";
            break;
        case wgpu::TextureFormat::RGBA32Sint:
            return "rgba32sint";
            break;
        case wgpu::TextureFormat::RGBA32Float:
            return "rgba32float";
            break;

        default:
            assert(false && "undefined");
            throw std::exception();
            break;
    }
}

// MARK: - BuiltInType
std::string toString(BuiltInType type) {
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

std::string toType(BuiltInType type) {
    switch (type) {
        case BuiltInType::VertexIndex:
            return "u32";
            break;
        case BuiltInType::InstanceIndex:
            return "u32";
            break;
        case BuiltInType::Position:
            return "vec4<f32>";
            break;
        case BuiltInType::FrontFacing:
            return "bool";
            break;
        case BuiltInType::FragDepth:
            return "f32";
            break;
        case BuiltInType::LocalInvocationID:
            return "vec3<u32>";
            break;
        case BuiltInType::LocalInvocationIndex:
            return "u32";
            break;
        case BuiltInType::GlobalInvocationID:
            return "vec3<u32>";
            break;
        case BuiltInType::WorkgroupID:
            return "vec3<u32>";
            break;
        case BuiltInType::NumWorkgroups:
            return "vec3<u32>";
            break;
        case BuiltInType::SampleIndex:
            return "u32";
            break;
        case BuiltInType::SampleMask:
            return "u32";
            break;
        default:
            break;
    }
}

}  // namespace vox
