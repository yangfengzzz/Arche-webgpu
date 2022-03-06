//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_common_h
#define wgsl_common_h

#include <string>
#include <vector>
#include <webgpu/webgpu_cpp.h>

namespace vox {
//MARK: - Attributes
enum class Attributes : uint32_t {
    Position = 0,
    Normal,
    UV_0,
    Tangent,
    Bitangent,
    Color_0,
    Weights_0,
    Joints_0,
    UV_1,
    UV_2,
    UV_3,
    UV_4,
    UV_5,
    UV_6,
    UV_7,
    TOTAL_COUNT
};
std::string toString(Attributes type);

//MARK: - UniformType
enum class UniformType {
    F32,
    I32,
    U32,
    
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
std::string toString(UniformType type);

//MARK: - TextureType
enum class TextureType {
    Texture1Df32,
    Texture1Di32,
    Texture1Du32,
    
    Texture2Df32,
    Texture2Di32,
    Texture2Du32,
    
    Texture2DArrayf32,
    Texture2DArrayi32,
    Texture2DArrayu32,
    
    Texture3Df32,
    Texture3Di32,
    Texture3Du32,
    
    TextureCubef32,
    TextureCubei32,
    TextureCubeu32,
    
    TextureCubeArrayf32,
    TextureCubeArrayi32,
    TextureCubeArrayu32,
    
    TextureMultisampled2Df32,
    TextureMultisampled2Di32,
    TextureMultisampled2Du32,
    
    TextureDepth2D,
    TextureDepth2DArray,
    TextureDepthCube,
    TextureDepthCubeArray,
    TextureDepthMultisampled2D,
};
std::string toString(TextureType type);
bool isMultisampled(TextureType type);
wgpu::TextureViewDimension viewDimension(TextureType type);
wgpu::TextureSampleType sampleType(TextureType type);

//MARK: - SamplerType
enum class SamplerType {
    Sampler,
    SamplerComparison
};
std::string toString(SamplerType type);
wgpu::SamplerBindingType bindingType(SamplerType type);

//MARK: - StorageTextureType
enum class StorageTextureType {
    TextureStorage1D,
    TextureStorage2D,
    TextureStorage2DArray,
    TextureStorage3D
};
std::string toString(StorageTextureType type);
wgpu::TextureViewDimension viewDimension(StorageTextureType type);

//MARK: - TexelFormats
std::string toString(wgpu::TextureFormat type);

//MARK: - BuiltInType
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
std::string toString(BuiltInType type);

std::string toType(BuiltInType type);

}
#endif /* wgsl_common_h */
