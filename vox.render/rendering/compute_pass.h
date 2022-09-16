//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/rendering/render_context.h"
#include "vox.render/rendering/resource_cache.h"
#include "vox.render/scene.h"

namespace vox {
class ComputePass {
public:
    using BindGroupLayoutEntryVecMap = std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>>;
    using BindGroupLayoutDescriptorMap = std::unordered_map<uint32_t, wgpu::BindGroupLayoutDescriptor>;

    ComputePass(wgpu::Device& device, std::shared_ptr<ShaderSource>&& source);

    ComputePass(const ComputePass&) = delete;

    ComputePass(ComputePass&&) = delete;

    virtual ~ComputePass() = default;

    ComputePass& operator=(const ComputePass&) = delete;

    ComputePass& operator=(ComputePass&&) = delete;

public:
    [[nodiscard]] uint32_t workgroupCountX() const;

    [[nodiscard]] uint32_t workgroupCountY() const;

    [[nodiscard]] uint32_t workgroupCountZ() const;

    void setDispatchCount(uint32_t workgroupCountX, uint32_t workgroupCountY = 1, uint32_t workgroupCountZ = 1);

    void attachShaderData(ShaderData* data);

    void detachShaderData(ShaderData* data);

    /**
     * @brief Compute virtual function
     * @param commandEncoder CommandEncoder to use to record compute commands
     */
    virtual void compute(wgpu::ComputePassEncoder& commandEncoder);

protected:
    uint32_t _workgroupCountX = 1;
    uint32_t _workgroupCountY = 1;
    uint32_t _workgroupCountZ = 1;

    std::vector<ShaderData*> _data{};
    std::shared_ptr<ShaderSource> _source{};

    std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> _bindGroupLayoutEntryVecMap;
    std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> _bindGroupEntryVecMap;
    wgpu::BindGroupDescriptor _bindGroupDescriptor;
    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDescriptor;

    wgpu::ComputePipelineDescriptor _computePipelineDescriptor;
    std::vector<wgpu::BindGroupEntry> _bindGroupEntries{};

    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;
};

}  // namespace vox