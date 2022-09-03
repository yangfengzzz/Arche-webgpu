//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shader/spirv_reflection.h"

#include "vox.base/logging.h"

namespace vox {
namespace {
template <ShaderResourceType T>
inline void ReadShaderResource(const spirv_cross::Compiler &compiler,
                               wgpu::ShaderStage stage,
                               std::vector<ShaderResource> &resources,
                               const ShaderVariant &variant) {
    LOGE("Not implemented! Read shader resources of type.")
}

template <spv::Decoration T>
inline void ReadResourceDecoration(const spirv_cross::Compiler &compiler /*compiler*/,
                                   const spirv_cross::Resource &resource /*resource*/,
                                   ShaderResource &shader_resource /*shader_resource*/,
                                   const ShaderVariant &variant /* variant */) {
    LOGE("Not implemented! Read resources decoration of type.")
}

template <>
inline void ReadResourceDecoration<spv::DecorationLocation>(const spirv_cross::Compiler &compiler,
                                                            const spirv_cross::Resource &resource,
                                                            ShaderResource &shader_resource,
                                                            const ShaderVariant &variant) {
    shader_resource.location = compiler.get_decoration(resource.id, spv::DecorationLocation);
}

template <>
inline void ReadResourceDecoration<spv::DecorationDescriptorSet>(const spirv_cross::Compiler &compiler,
                                                                 const spirv_cross::Resource &resource,
                                                                 ShaderResource &shader_resource,
                                                                 const ShaderVariant &variant) {
    shader_resource.set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
}

template <>
inline void ReadResourceDecoration<spv::DecorationBinding>(const spirv_cross::Compiler &compiler,
                                                           const spirv_cross::Resource &resource,
                                                           ShaderResource &shader_resource,
                                                           const ShaderVariant &variant) {
    shader_resource.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
}

template <>
inline void ReadResourceDecoration<spv::DecorationInputAttachmentIndex>(const spirv_cross::Compiler &compiler,
                                                                        const spirv_cross::Resource &resource,
                                                                        ShaderResource &shader_resource,
                                                                        const ShaderVariant &variant) {
    shader_resource.input_attachment_index = compiler.get_decoration(resource.id, spv::DecorationInputAttachmentIndex);
}

template <>
inline void ReadResourceDecoration<spv::DecorationNonWritable>(const spirv_cross::Compiler &compiler,
                                                               const spirv_cross::Resource &resource,
                                                               ShaderResource &shader_resource,
                                                               const ShaderVariant &variant) {
    shader_resource.qualifiers |= ShaderResourceQualifiers::NON_WRITABLE;
}

template <>
inline void ReadResourceDecoration<spv::DecorationNonReadable>(const spirv_cross::Compiler &compiler,
                                                               const spirv_cross::Resource &resource,
                                                               ShaderResource &shader_resource,
                                                               const ShaderVariant &variant) {
    shader_resource.qualifiers |= ShaderResourceQualifiers::NON_READABLE;
}

inline void ReadResourceVecSize(const spirv_cross::Compiler &compiler,
                                const spirv_cross::Resource &resource,
                                ShaderResource &shader_resource,
                                const ShaderVariant &variant) {
    const auto &spirv_type = compiler.get_type_from_variable(resource.id);

    shader_resource.vec_size = spirv_type.vecsize;
    shader_resource.columns = spirv_type.columns;
}

inline void ReadResourceArraySize(const spirv_cross::Compiler &compiler,
                                  const spirv_cross::Resource &resource,
                                  ShaderResource &shader_resource,
                                  const ShaderVariant &variant) {
    const auto &spirv_type = compiler.get_type_from_variable(resource.id);

    shader_resource.array_size = !spirv_type.array.empty() ? spirv_type.array[0] : 1;
}

inline void ReadResourceSize(const spirv_cross::Compiler &compiler,
                             const spirv_cross::Resource &resource,
                             ShaderResource &shader_resource,
                             const ShaderVariant &variant) {
    const auto &spirv_type = compiler.get_type_from_variable(resource.id);

    size_t array_size = 0;
    if (variant.GetRuntimeArraySizes().count(resource.name) != 0) {
        array_size = variant.GetRuntimeArraySizes().at(resource.name);
    }

    shader_resource.size = utility::ToU32(compiler.get_declared_struct_size_runtime_array(spirv_type, array_size));
}

inline void ReadResourceSize(const spirv_cross::Compiler &compiler,
                             const spirv_cross::SPIRConstant &constant,
                             ShaderResource &shader_resource,
                             const ShaderVariant &variant) {
    auto spirv_type = compiler.get_type(constant.constant_type);

    switch (spirv_type.basetype) {
        case spirv_cross::SPIRType::BaseType::Boolean:
        case spirv_cross::SPIRType::BaseType::Char:
        case spirv_cross::SPIRType::BaseType::Int:
        case spirv_cross::SPIRType::BaseType::UInt:
        case spirv_cross::SPIRType::BaseType::Float:
            shader_resource.size = 4;
            break;
        case spirv_cross::SPIRType::BaseType::Int64:
        case spirv_cross::SPIRType::BaseType::UInt64:
        case spirv_cross::SPIRType::BaseType::Double:
            shader_resource.size = 8;
            break;
        default:
            shader_resource.size = 0;
            break;
    }
}

template <>
inline void ReadShaderResource<ShaderResourceType::INPUT>(const spirv_cross::Compiler &compiler,
                                                          wgpu::ShaderStage stage,
                                                          std::vector<ShaderResource> &resources,
                                                          const ShaderVariant &variant) {
    auto input_resources = compiler.get_shader_resources().stage_inputs;

    for (auto &resource : input_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::INPUT;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;

        ReadResourceVecSize(compiler, resource, shader_resource, variant);
        ReadResourceArraySize(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationLocation>(compiler, resource, shader_resource, variant);

        resources.push_back(shader_resource);
    }
}

template <>
inline void ReadShaderResource<ShaderResourceType::INPUT_ATTACHMENT>(const spirv_cross::Compiler &compiler,
                                                                     wgpu::ShaderStage /*stage*/ stage,
                                                                     std::vector<ShaderResource> &resources,
                                                                     const ShaderVariant &variant) {
    auto subpass_resources = compiler.get_shader_resources().subpass_inputs;

    for (auto &resource : subpass_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::INPUT_ATTACHMENT;
        shader_resource.stages = wgpu::ShaderStage::Fragment;
        shader_resource.name = resource.name;

        ReadResourceArraySize(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationInputAttachmentIndex>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

        resources.push_back(shader_resource);
    }
}

template <>
inline void ReadShaderResource<ShaderResourceType::OUTPUT>(const spirv_cross::Compiler &compiler,
                                                           wgpu::ShaderStage stage,
                                                           std::vector<ShaderResource> &resources,
                                                           const ShaderVariant &variant) {
    auto output_resources = compiler.get_shader_resources().stage_outputs;

    for (auto &resource : output_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::OUTPUT;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;

        ReadResourceArraySize(compiler, resource, shader_resource, variant);
        ReadResourceVecSize(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationLocation>(compiler, resource, shader_resource, variant);

        resources.push_back(shader_resource);
    }
}

template <>
inline void ReadShaderResource<ShaderResourceType::IMAGE>(const spirv_cross::Compiler &compiler,
                                                          wgpu::ShaderStage stage,
                                                          std::vector<ShaderResource> &resources,
                                                          const ShaderVariant &variant) {
    auto image_resources = compiler.get_shader_resources().separate_images;

    for (auto &resource : image_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::IMAGE;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;

        ReadResourceArraySize(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

        resources.push_back(shader_resource);
    }
}

template <>
inline void ReadShaderResource<ShaderResourceType::IMAGE_SAMPLER>(const spirv_cross::Compiler &compiler,
                                                                  wgpu::ShaderStage stage,
                                                                  std::vector<ShaderResource> &resources,
                                                                  const ShaderVariant &variant) {
    auto image_resources = compiler.get_shader_resources().sampled_images;

    for (auto &resource : image_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::IMAGE_SAMPLER;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;

        ReadResourceArraySize(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

        resources.push_back(shader_resource);
    }
}

template <>
inline void ReadShaderResource<ShaderResourceType::IMAGE_STORAGE>(const spirv_cross::Compiler &compiler,
                                                                  wgpu::ShaderStage stage,
                                                                  std::vector<ShaderResource> &resources,
                                                                  const ShaderVariant &variant) {
    auto storage_resources = compiler.get_shader_resources().storage_images;

    for (auto &resource : storage_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::IMAGE_STORAGE;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;

        ReadResourceArraySize(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationNonReadable>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationNonWritable>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

        resources.push_back(shader_resource);
    }
}

template <>
inline void ReadShaderResource<ShaderResourceType::SAMPLER>(const spirv_cross::Compiler &compiler,
                                                            wgpu::ShaderStage stage,
                                                            std::vector<ShaderResource> &resources,
                                                            const ShaderVariant &variant) {
    auto sampler_resources = compiler.get_shader_resources().separate_samplers;

    for (auto &resource : sampler_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::SAMPLER;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;

        ReadResourceArraySize(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

        resources.push_back(shader_resource);
    }
}

template <>
inline void ReadShaderResource<ShaderResourceType::BUFFER_UNIFORM>(const spirv_cross::Compiler &compiler,
                                                                   wgpu::ShaderStage stage,
                                                                   std::vector<ShaderResource> &resources,
                                                                   const ShaderVariant &variant) {
    auto uniform_resources = compiler.get_shader_resources().uniform_buffers;

    for (auto &resource : uniform_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::BUFFER_UNIFORM;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;

        ReadResourceSize(compiler, resource, shader_resource, variant);
        ReadResourceArraySize(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

        resources.push_back(shader_resource);
    }
}

template <>
inline void ReadShaderResource<ShaderResourceType::BUFFER_STORAGE>(const spirv_cross::Compiler &compiler,
                                                                   wgpu::ShaderStage stage,
                                                                   std::vector<ShaderResource> &resources,
                                                                   const ShaderVariant &variant) {
    auto storage_resources = compiler.get_shader_resources().storage_buffers;

    for (auto &resource : storage_resources) {
        ShaderResource shader_resource;
        shader_resource.type = ShaderResourceType::BUFFER_STORAGE;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;

        ReadResourceSize(compiler, resource, shader_resource, variant);
        ReadResourceArraySize(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationNonReadable>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationNonWritable>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        ReadResourceDecoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

        resources.push_back(shader_resource);
    }
}
}  // namespace

bool SpirvReflection::ReflectShaderResources(wgpu::ShaderStage stage,
                                             const std::vector<uint32_t> &spirv,
                                             std::vector<ShaderResource> &resources,
                                             const ShaderVariant &variant) {
    spirv_cross::CompilerGLSL compiler{spirv};

    auto opts = compiler.get_common_options();
    opts.enable_420pack_extension = true;

    compiler.set_common_options(opts);

    ParseShaderResources(compiler, stage, resources, variant);
    ParsePushConstants(compiler, stage, resources, variant);
    ParseSpecializationConstants(compiler, stage, resources, variant);

    return true;
}

void SpirvReflection::ParseShaderResources(const spirv_cross::Compiler &compiler,
                                           wgpu::ShaderStage stage,
                                           std::vector<ShaderResource> &resources,
                                           const ShaderVariant &variant) {
    ReadShaderResource<ShaderResourceType::INPUT>(compiler, stage, resources, variant);
    ReadShaderResource<ShaderResourceType::INPUT_ATTACHMENT>(compiler, stage, resources, variant);
    ReadShaderResource<ShaderResourceType::OUTPUT>(compiler, stage, resources, variant);
    ReadShaderResource<ShaderResourceType::IMAGE>(compiler, stage, resources, variant);
    ReadShaderResource<ShaderResourceType::IMAGE_SAMPLER>(compiler, stage, resources, variant);
    ReadShaderResource<ShaderResourceType::IMAGE_STORAGE>(compiler, stage, resources, variant);
    ReadShaderResource<ShaderResourceType::SAMPLER>(compiler, stage, resources, variant);
    ReadShaderResource<ShaderResourceType::BUFFER_UNIFORM>(compiler, stage, resources, variant);
    ReadShaderResource<ShaderResourceType::BUFFER_STORAGE>(compiler, stage, resources, variant);
}

void SpirvReflection::ParsePushConstants(const spirv_cross::Compiler &compiler,
                                         wgpu::ShaderStage stage,
                                         std::vector<ShaderResource> &resources,
                                         const ShaderVariant &variant) {
    auto shader_resources = compiler.get_shader_resources();

    for (auto &resource : shader_resources.push_constant_buffers) {
        const auto &spirv_type = compiler.get_type_from_variable(resource.id);

        std::uint32_t offset = std::numeric_limits<std::uint32_t>::max();

        for (auto i = 0U; i < spirv_type.member_types.size(); ++i) {
            auto mem_offset = compiler.get_member_decoration(spirv_type.self, i, spv::DecorationOffset);

            offset = std::min(offset, mem_offset);
        }

        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::PUSH_CONSTANT;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;
        shader_resource.offset = offset;

        ReadResourceSize(compiler, resource, shader_resource, variant);

        shader_resource.size -= shader_resource.offset;

        resources.push_back(shader_resource);
    }
}

void SpirvReflection::ParseSpecializationConstants(const spirv_cross::Compiler &compiler,
                                                   wgpu::ShaderStage stage,
                                                   std::vector<ShaderResource> &resources,
                                                   const ShaderVariant &variant) {
    auto specialization_constants = compiler.get_specialization_constants();

    for (auto &resource : specialization_constants) {
        auto &spirv_value = compiler.get_constant(resource.id);

        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::SPECIALIZATION_CONSTANT;
        shader_resource.stages = stage;
        shader_resource.name = compiler.get_name(resource.id);
        shader_resource.offset = 0;
        shader_resource.constant_id = resource.constant_id;

        ReadResourceSize(compiler, spirv_value, shader_resource, variant);

        resources.push_back(shader_resource);
    }
}

}  // namespace vox
