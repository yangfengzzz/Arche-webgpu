//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <webgpu/webgpu_cpp.h>

#include <unordered_map>
#include "vox.render/singleton.h"
#include "vox.render/shader/shader_module.h"

namespace vox {
/**
 * @brief Struct to hold the internal state of the Resource Cache
 *
 */
struct ResourceCacheState {
    std::unordered_map<std::size_t, wgpu::PipelineLayout> pipelineLayouts;

    std::unordered_map<std::size_t, wgpu::BindGroupLayout> bindGroupLayouts;

    std::unordered_map<std::size_t, wgpu::BindGroup> bindGroups;

    std::unordered_map<std::size_t, wgpu::RenderPipeline> renderPipelines;

    std::unordered_map<std::size_t, wgpu::ComputePipeline> computePipelines;

    std::unordered_map<std::size_t, wgpu::Sampler> samplers;

    std::unordered_map<std::size_t, std::unique_ptr<ShaderModule>> shaderModules;
};

/**
 * @brief Cache all sorts of WebGPU objects specific to a WebGPU device.
 * Supports serialization and deserialization of cached resources.
 * There is only one cache for all these objects, with several unordered_map of hash indices
 * and objects. For every object requested, there is a templated version on request_resource.
 * Some objects may need building if they are not found in the cache.
 *
 * The resource cache is also linked with ResourceRecord and ResourceReplay. Replay can warm-up
 * the cache on app startup by creating all necessary objects.
 * The cache holds pointers to objects and has a mapping from such pointers to hashes.
 * It can only be destroyed in bulk, single elements cannot be removed.
 */
class ResourceCache : public Singleton<ResourceCache> {
public:
    static ResourceCache &GetSingleton();

    static ResourceCache *GetSingletonPtr();

    explicit ResourceCache(wgpu::Device &device);

    ResourceCache(const ResourceCache &) = delete;

    ResourceCache(ResourceCache &&) = delete;

    ResourceCache &operator=(const ResourceCache &) = delete;

    ResourceCache &operator=(ResourceCache &&) = delete;

public:
    wgpu::BindGroupLayout &requestBindGroupLayout(const wgpu::BindGroupLayoutDescriptor &descriptor);

    wgpu::PipelineLayout &requestPipelineLayout(const wgpu::PipelineLayoutDescriptor &descriptor);

    wgpu::BindGroup &requestBindGroup(const wgpu::BindGroupDescriptor &descriptor);

    wgpu::RenderPipeline &requestPipeline(const wgpu::RenderPipelineDescriptor &descriptor);

    wgpu::ComputePipeline &requestPipeline(const wgpu::ComputePipelineDescriptor &descriptor);

    wgpu::Sampler &requestSampler(const wgpu::SamplerDescriptor &descriptor);

    ShaderModule &requestShaderModule(wgpu::ShaderStage stage,
                                      const ShaderSource &glsl_source,
                                      const ShaderVariant &shader_variant);

private:
    wgpu::Device &_device;

    ResourceCacheState _state;
};
template <>
inline ResourceCache *Singleton<ResourceCache>::ms_singleton{nullptr};

}  // namespace vox