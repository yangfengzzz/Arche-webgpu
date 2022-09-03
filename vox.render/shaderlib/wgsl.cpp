//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shaderlib/wgsl.h"

namespace vox {
WGSL::WGSL(const std::string& source, const BindGroupInfo& info, const BindGroupLayoutEntryMap& entryMap)
    : _source(source), _bindGroupInfo(info), _bindGroupLayoutEntryMap(entryMap) {}

std::pair<const std::string&, const WGSL::BindGroupInfo&> WGSL::compile(const ShaderMacroCollection& macros) {
    return std::make_pair(_source, _bindGroupInfo);
}

const WGSL::BindGroupLayoutEntryMap& WGSL::bindGroupLayoutEntryMap() { return _bindGroupLayoutEntryMap; }

WGSLEncoder WGSL::createSourceEncoder(wgpu::ShaderStage currentStage) { return WGSLEncoder(this, currentStage); }

void WGSL::_setSource(const std::string& source) { _source = source; }

void WGSL::_setBindGroupInfo(const BindGroupInfo& info) { _bindGroupInfo = info; }

void WGSL::_setBindGroupLayoutEntryMap(const BindGroupLayoutEntryMap& map) {
    for (const auto& groupIter : map) {
        auto iter = _bindGroupLayoutEntryMap.find(groupIter.first);
        if (iter != _bindGroupLayoutEntryMap.end()) {
            for (const auto& bindingIter : groupIter.second) {
                auto iter = _bindGroupLayoutEntryMap[groupIter.first].find(bindingIter.first);
                if (iter == _bindGroupLayoutEntryMap[groupIter.first].end()) {
                    _bindGroupLayoutEntryMap[groupIter.first][bindingIter.first] = bindingIter.second;
                }
            }
        } else {
            _bindGroupLayoutEntryMap[groupIter.first] = groupIter.second;
        }
    }
}

}  // namespace vox
