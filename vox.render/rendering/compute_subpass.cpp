//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "compute_subpass.h"
#include "render_pass.h"

namespace vox {
Subpass::Type ComputeSubpass::type() {
    return Subpass::Type::Compute;
}

ComputeSubpass::ComputeSubpass(WGSLPtr&& source) :
Subpass(),
_source(std::move(source)) {
}


void ComputeSubpass::prepare() {
    
}

void ComputeSubpass::compute(wgpu::ComputePassEncoder &commandEncoder) {
    
}


//MARK: - Internal
void ComputeSubpass::_flush() {
    _bindGroupLayoutEntryVecMap.clear();
    _bindGroupLayoutDescriptorMap.clear();
}

wgpu::ShaderModule &ComputeSubpass::_compileShader(const ShaderMacroCollection& macros) {
    auto result = _source->compile(macros);
//    std::cout<<result.first<<std::endl;
    
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
    
    return _pass->resourceCache().requestShader(result.first);
}

wgpu::BindGroupLayoutEntry ComputeSubpass::_findEntry(uint32_t group, uint32_t binding) {
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

}
