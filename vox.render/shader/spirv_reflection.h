//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <spirv_glsl.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "vox.render/shader/shader_module.h"

namespace vox {
/// Generate a list of shader resource based on SPIRV reflection code, and provided ShaderVariant
class SpirvReflection {
public:
    /// @brief Reflects shader resources from SPIRV code
    /// @param stage The Vulkan shader stage flag
    /// @param spirv The SPIRV code of shader
    /// @param[out] resources The list of reflected shader resources
    /// @param variant ShaderVariant used for reflection to specify the size of the runtime arrays in Storage Buffers
    static bool ReflectShaderResources(wgpu::ShaderStage stage,
                                       const std::vector<uint32_t> &spirv,
                                       std::unordered_map<std::string, ShaderResource> &resources,
                                       const ShaderVariant &variant);

private:
    static void ParseShaderResources(const spirv_cross::Compiler &compiler,
                                     wgpu::ShaderStage stage,
                                     std::unordered_map<std::string, ShaderResource> &resources,
                                     const ShaderVariant &variant);

    static void ParsePushConstants(const spirv_cross::Compiler &compiler,
                                   wgpu::ShaderStage stage,
                                   std::unordered_map<std::string, ShaderResource> &resources,
                                   const ShaderVariant &variant);

    static void ParseSpecializationConstants(const spirv_cross::Compiler &compiler,
                                             wgpu::ShaderStage stage,
                                             std::unordered_map<std::string, ShaderResource> &resources,
                                             const ShaderVariant &variant);
};

}  // namespace vox
