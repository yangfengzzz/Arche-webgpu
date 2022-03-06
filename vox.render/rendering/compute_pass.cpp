//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "compute_pass.h"
#include "render_pass.h"
#include <glog/logging.h>

namespace vox {
ComputePass::ComputePass(wgpu::Device& device, WGSLPtr&& source) :
_source(std::move(source)),
_resourceCache(device) {
    _computePipelineDescriptor.compute.entryPoint = "main";
}

void ComputePass::setDispatchCount(uint32_t workgroupCountX,
                                      uint32_t workgroupCountY,
                                      uint32_t workgroupCountZ) {
    _workgroupCountX = workgroupCountX;
    _workgroupCountY = workgroupCountY;
    _workgroupCountZ = workgroupCountZ;
}

void ComputePass::attachShaderData(ShaderData* data) {
    auto iter = std::find(_data.begin(), _data.end(), data);
    if (iter == _data.end()) {
        _data.push_back(data);
    } else {
        LOG(ERROR) << "ShaderData already attached." << std::endl;;
    }
}

void ComputePass::detachShaderData(ShaderData* data) {
    auto iter = std::find(_data.begin(), _data.end(), data);
    if (iter != _data.end()) {
        _data.erase(iter);
    }
}

void ComputePass::compute(wgpu::ComputePassEncoder &passEncoder) {
    auto compileMacros = ShaderMacroCollection();
    for (auto& shaderData : _data) {
        shaderData->mergeMacro(compileMacros, compileMacros);
    }
    
    _computePipelineDescriptor.compute.module = _compileShader(compileMacros);
    
    std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
    for (auto& layoutDesc : _bindGroupLayoutDescriptorMap) {
        wgpu::BindGroupLayout bindGroupLayout = _resourceCache.requestBindGroupLayout(layoutDesc.second);
        _bindGroupEntries.clear();
        _bindGroupEntries.resize(layoutDesc.second.entryCount);
        for (uint32_t i = 0; i < layoutDesc.second.entryCount; i++) {
            auto& entry = layoutDesc.second.entries[i];
            _bindGroupEntries[i].binding = entry.binding;
            if (entry.buffer.type != wgpu::BufferBindingType::Undefined) {
                _bindingData(_bindGroupEntries[i]);
            } else if (entry.texture.sampleType != wgpu::TextureSampleType::Undefined
                       || entry.storageTexture.access != wgpu::StorageTextureAccess::Undefined) {
                _bindingTexture(_bindGroupEntries[i]);
            } else if (entry.sampler.type != wgpu::SamplerBindingType::Undefined) {
                _bindingSampler(_bindGroupEntries[i]);
            }
        }
        _bindGroupDescriptor.layout = bindGroupLayout;
        _bindGroupDescriptor.entryCount = layoutDesc.second.entryCount;
        _bindGroupDescriptor.entries = _bindGroupEntries.data();
        auto uniformBindGroup = _resourceCache.requestBindGroup(_bindGroupDescriptor);
        passEncoder.SetBindGroup(layoutDesc.first, uniformBindGroup);
        bindGroupLayouts.emplace_back(std::move(bindGroupLayout));
    }
    _flush();
    
    _pipelineLayoutDescriptor.bindGroupLayoutCount = static_cast<uint32_t>(bindGroupLayouts.size());
    _pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
    _pipelineLayout = _resourceCache.requestPipelineLayout(_pipelineLayoutDescriptor);
    _computePipelineDescriptor.layout = _pipelineLayout;
    auto renderPipeline = _resourceCache.requestPipeline(_computePipelineDescriptor);
    passEncoder.SetPipeline(renderPipeline);
    
    passEncoder.Dispatch(_workgroupCountX, _workgroupCountY, _workgroupCountZ);
}

//MARK: - Internal
void ComputePass::_flush() {
    _bindGroupLayoutEntryVecMap.clear();
    _bindGroupLayoutDescriptorMap.clear();
}

wgpu::ShaderModule &ComputePass::_compileShader(const ShaderMacroCollection& macros) {
    auto result = _source->compile(macros);
    // std::cout<<result.first<<std::endl;
    
    for (const auto& info : result.second) {
        _bindGroupLayoutEntryVecMap[info.first].reserve(info.second.size());
        for (const auto& entry : info.second) {
            _bindGroupLayoutEntryVecMap[info.first].push_back(_findEntry(info.first, entry));
        }
    }
    
    for (const auto& entryVec : _bindGroupLayoutEntryVecMap) {
        wgpu::BindGroupLayoutDescriptor desc;
        desc.entryCount = static_cast<uint32_t>(entryVec.second.size());
        desc.entries = entryVec.second.data();
        _bindGroupLayoutDescriptorMap[entryVec.first] = desc;
    }
    
    return _resourceCache.requestShader(result.first);
}

wgpu::BindGroupLayoutEntry ComputePass::_findEntry(uint32_t group, uint32_t binding) {
    wgpu::BindGroupLayoutEntry entry;
    entry.visibility = wgpu::ShaderStage::None;
    
    const auto& entryMap = _source->bindGroupLayoutEntryMap();
    auto iter = entryMap.find(group);
    if (iter != entryMap.end()) {
        auto entryIter = iter->second.find(binding);
        if (entryIter != iter->second.end()) {
            entry = entryIter->second;
        }
    }
    
    if (entry.visibility != wgpu::ShaderStage::None) {
        return entry;
    } else {
        assert(false);
        throw std::exception();
    }
}

void ComputePass::_bindingData(wgpu::BindGroupEntry& entry) {
    for (auto shaderData : _data) {
        auto buffer = shaderData->getData(entry.binding);
        if (buffer) {
            entry.buffer = buffer->handle();
            entry.size = buffer->size();
            break;
        }
    }
}

void ComputePass::_bindingTexture(wgpu::BindGroupEntry& entry) {
    for (auto shaderData : _data) {
        auto texture = shaderData->getTextureView(entry.binding);
        if (texture) {
            entry.textureView = texture.value();
            break;
        }
    }
}

void ComputePass::_bindingSampler(wgpu::BindGroupEntry& entry) {
    for (auto shaderData : _data) {
        auto sampler = shaderData->getSampler(entry.binding);
        if (sampler) {
            entry.sampler = sampler.value();
            break;
        }
    }
}

}
