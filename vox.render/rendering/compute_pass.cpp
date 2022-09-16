//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/compute_pass.h"

#include "vox.base/logging.h"

namespace vox {
ComputePass::ComputePass(wgpu::Device& device, std::shared_ptr<ShaderSource>&& source)
    : _source(std::move(source)), _resourceCache(device) {
    _computePipelineDescriptor.compute.entryPoint = "main";
}

uint32_t ComputePass::workgroupCountX() const { return _workgroupCountX; }

uint32_t ComputePass::workgroupCountY() const { return _workgroupCountY; }

uint32_t ComputePass::workgroupCountZ() const { return _workgroupCountZ; }

void ComputePass::setDispatchCount(uint32_t workgroupCountX, uint32_t workgroupCountY, uint32_t workgroupCountZ) {
    _workgroupCountX = workgroupCountX;
    _workgroupCountY = workgroupCountY;
    _workgroupCountZ = workgroupCountZ;
}

void ComputePass::attachShaderData(ShaderData* data) {
    auto iter = std::find(_data.begin(), _data.end(), data);
    if (iter == _data.end()) {
        _data.push_back(data);
    } else {
        LOGE("ShaderData already attached.")
    }
}

void ComputePass::detachShaderData(ShaderData* data) {
    auto iter = std::find(_data.begin(), _data.end(), data);
    if (iter != _data.end()) {
        _data.erase(iter);
    }
}

void ComputePass::compute(wgpu::ComputePassEncoder& passEncoder) {
    auto compileMacros = ShaderVariant();
    for (auto& shaderData : _data) {
        shaderData->mergeVariants(compileMacros, compileMacros);
    }

    auto& shader_module = _resourceCache.requestShaderModule(wgpu::ShaderStage::Compute, *_source, compileMacros);
    _computePipelineDescriptor.compute.module = shader_module.handle();

    _bindGroupLayoutEntryVecMap.clear();
    _bindGroupEntryVecMap.clear();
    for (auto& shaderData : _data) {
        shaderData->bindData(shader_module.GetResources(), _bindGroupLayoutEntryVecMap, _bindGroupEntryVecMap);
    }

    std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
    for (const auto& bindGroupLayoutEntryVec : _bindGroupLayoutEntryVecMap) {
        bindGroupLayoutDescriptor.entries = bindGroupLayoutEntryVec.second.data();
        bindGroupLayoutDescriptor.entryCount = bindGroupLayoutEntryVec.second.size();
        wgpu::BindGroupLayout bindGroupLayout = _resourceCache.requestBindGroupLayout(bindGroupLayoutDescriptor);

        const auto group = bindGroupLayoutEntryVec.first;
        const auto& bindGroupEntryVec = _bindGroupEntryVecMap[group];
        _bindGroupDescriptor.layout = bindGroupLayout;
        _bindGroupDescriptor.entryCount = bindGroupEntryVec.size();
        _bindGroupDescriptor.entries = bindGroupEntryVec.data();
        auto uniformBindGroup = _resourceCache.requestBindGroup(_bindGroupDescriptor);
        passEncoder.SetBindGroup(group, uniformBindGroup);
        bindGroupLayouts.emplace_back(std::move(bindGroupLayout));
    }

    _pipelineLayoutDescriptor.bindGroupLayoutCount = static_cast<uint32_t>(bindGroupLayouts.size());
    _pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
    _pipelineLayout = _resourceCache.requestPipelineLayout(_pipelineLayoutDescriptor);
    _computePipelineDescriptor.layout = _pipelineLayout;
    auto renderPipeline = _resourceCache.requestPipeline(_computePipelineDescriptor);
    passEncoder.SetPipeline(renderPipeline);

    passEncoder.DispatchWorkgroups(_workgroupCountX, _workgroupCountY, _workgroupCountZ);
}

}  // namespace vox
