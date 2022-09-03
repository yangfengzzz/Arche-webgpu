//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef resource_cache_hpp
#define resource_cache_hpp

#include <webgpu/webgpu_cpp.h>

#include <unordered_map>

namespace vox {
/**
 * @brief Struct to hold the internal state of the Resource Cache
 *
 */
struct ResourceCacheState {
    std::unordered_map<std::size_t, wgpu::BindGroupLayout> bindGroupLayouts;
    std::unordered_map<std::size_t, wgpu::PipelineLayout> pipelineLayouts;
    std::unordered_map<std::size_t, wgpu::RenderPipeline> renderPipelines;
    std::unordered_map<std::size_t, wgpu::BindGroup> bindGroups;

    std::unordered_map<std::size_t, wgpu::ComputePipeline> computePipelines;

    std::unordered_map<std::size_t, wgpu::ShaderModule> shaders;
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
class ResourceCache {
public:
    ResourceCache(wgpu::Device &device);

    ResourceCache(const ResourceCache &) = delete;

    ResourceCache(ResourceCache &&) = delete;

    ResourceCache &operator=(const ResourceCache &) = delete;

    ResourceCache &operator=(ResourceCache &&) = delete;

public:
    wgpu::BindGroupLayout &requestBindGroupLayout(wgpu::BindGroupLayoutDescriptor &descriptor);

    wgpu::PipelineLayout &requestPipelineLayout(wgpu::PipelineLayoutDescriptor &descriptor);

    wgpu::BindGroup &requestBindGroup(wgpu::BindGroupDescriptor &descriptor);

    wgpu::RenderPipeline &requestPipeline(wgpu::RenderPipelineDescriptor &descriptor);

    wgpu::ComputePipeline &requestPipeline(wgpu::ComputePipelineDescriptor &descriptor);

    wgpu::ShaderModule &requestShader(const std::string &source);

private:
    wgpu::Device &_device;

    ResourceCacheState _state;
};

}  // namespace vox

#endif /* resource_cache_hpp */
