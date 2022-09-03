//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef compute_subpass_hpp
#define compute_subpass_hpp

#include "render_context.h"
#include "resource_cache.h"
#include "scene.h"

namespace vox {
class ComputePass {
public:
    using BindGroupLayoutEntryVecMap = std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>>;
    using BindGroupLayoutDescriptorMap = std::unordered_map<uint32_t, wgpu::BindGroupLayoutDescriptor>;

    ComputePass(wgpu::Device& device, WGSLPtr&& source);

    ComputePass(const ComputePass&) = delete;

    ComputePass(ComputePass&&) = delete;

    virtual ~ComputePass() = default;

    ComputePass& operator=(const ComputePass&) = delete;

    ComputePass& operator=(ComputePass&&) = delete;

public:
    uint32_t workgroupCountX() const;

    uint32_t workgroupCountY() const;

    uint32_t workgroupCountZ() const;

    void setDispatchCount(uint32_t workgroupCountX, uint32_t workgroupCountY = 1, uint32_t workgroupCountZ = 1);

    void attachShaderData(ShaderData* data);

    void detachShaderData(ShaderData* data);

    /**
     * @brief Compute virtual function
     * @param commandEncoder CommandEncoder to use to record compute commands
     */
    virtual void compute(wgpu::ComputePassEncoder& commandEncoder);

protected:
    wgpu::ShaderModule& _compileShader(const ShaderMacroCollection& macros);

    void _bindingData(wgpu::BindGroupEntry& entry);

    void _bindingTexture(wgpu::BindGroupEntry& entry);

    void _bindingSampler(wgpu::BindGroupEntry& entry);

    void _flush();

    uint32_t _workgroupCountX = 1;
    uint32_t _workgroupCountY = 1;
    uint32_t _workgroupCountZ = 1;

    std::vector<ShaderData*> _data{};
    WGSLPtr _source{};
    BindGroupLayoutEntryVecMap _bindGroupLayoutEntryVecMap{};
    BindGroupLayoutDescriptorMap _bindGroupLayoutDescriptorMap{};

    wgpu::ComputePipelineDescriptor _computePipelineDescriptor;
    wgpu::BindGroupDescriptor _bindGroupDescriptor;
    std::vector<wgpu::BindGroupEntry> _bindGroupEntries{};

    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;

private:
    wgpu::BindGroupLayoutEntry _findEntry(uint32_t group, uint32_t binding);

    ResourceCache _resourceCache;
};

}  // namespace vox
#endif /* compute_subpass_hpp */
