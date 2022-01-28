//
//  wgsl.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/28.
//

#include "wgsl.h"
#include "shader/shader.h"
#include <fmt/core.h>

namespace vox {
WGSL::WGSL(const std::string& source,
           const BindGroupLayoutEntryVecMap& vecMap):
_source(source),
_bindGroupLayoutEntryVecMap(vecMap){
}

const std::string& WGSL::compile(const ShaderMacroCollection& macros) {
    return _source;
}

const WGSL::BindGroupLayoutDescriptorMap& WGSL::bindGroupLayoutDescriptors() {
    for (const auto& entryVec : _bindGroupLayoutEntryVecMap) {
        auto iter = _bindGroupLayoutDescriptorMap.find(entryVec.first);
        if (iter != _bindGroupLayoutDescriptorMap.end()) {
            iter->second.entryCount = static_cast<uint32_t>(entryVec.second.size());
            iter->second.entries = entryVec.second.data();
        } else {
            wgpu::BindGroupLayoutDescriptor desc;
            desc.entryCount = static_cast<uint32_t>(entryVec.second.size());
            desc.entries = entryVec.second.data();
            _bindGroupLayoutDescriptorMap[entryVec.first] = desc;
        }
    }
    return _bindGroupLayoutDescriptorMap;
}

void WGSL::begin(wgpu::ShaderStage stage) {
    _currentStage = stage;
}

void WGSL::addStruct(const std::string& code) {
    _source += code;
}

void WGSL::addUniformBinding(const std::string& name, UniformType type, uint32_t group) {
    addUniformBinding(name, uniformTypeToString(type), group);
}

void WGSL::addUniformBinding(const std::string& name, const std::string& type, uint32_t group) {
    const std::string formatTemplate = "@group({}) @binding({})\n "
    "var<uniform> {}: {};\n ";
    
    auto property = Shader::getPropertyByName(name);
    if (property.has_value()) {
        uint32_t binding = property.value().uniqueId;
        _source += fmt::format(formatTemplate, group, binding,
                               name, type);
        
        wgpu::BindGroupLayoutEntry entry;
        entry.binding = binding;
        entry.visibility = _currentStage;
        entry.buffer.type = wgpu::BufferBindingType::Uniform;
        auto iter = _bindGroupLayoutEntryVecMap.find(group);
        if (iter == _bindGroupLayoutEntryVecMap.end()) {
            _bindGroupLayoutEntryVecMap[group] = {entry};
        } else {
            iter->second.push_back(entry);
        }
    } else {
        assert(false && "Unknown Uniform Name");
    }
}
    
void WGSL::beginInputStruct(const std::string& name) {
    _source += "struct ";
    _source += name;
    _source += " {\n";
    _inputTypeName.push_back(name);
}

void WGSL::addInputType(const std::string& code) {
    _source += code;
    _source += "\n";
}

void WGSL::endInputStruct() {
    _source += "}\n";
}

void WGSL::beginOutputStruct(const std::string& name) {
    _source += "struct ";
    _source += name;
    _source += " {\n";
    _outputTypeName = name;
}

void WGSL::addOutputType(const std::string& code) {
    _source += code;
    _source += "\n";
}

void WGSL::endOutputStruct() {
    _source += "}\n";
}

void WGSL::beginEntry(const std::initializer_list<std::string>& parameter) {
    if (_currentStage == wgpu::ShaderStage::Vertex) {
        _source += "@stage(vertex)\n";
    } else if (_currentStage == wgpu::ShaderStage::Fragment) {
        _source += "@stage(fragment)\n";
    } else {
        assert(false && "Use Begin at first");
    }
    
    auto inputIter = _inputTypeName.begin();
    _source += "fn main(";
    for (const auto& name : parameter) {
        if (inputIter != _inputTypeName.end()) {
            _source += name;
            _source += ": ";
            _source += *inputIter;
            _source += ", ";
            inputIter++;
        }
    }
    _source += ") -> ";
    _source += _outputTypeName;
    _source += " {\n";
}

void WGSL::addEntry(const std::string& code) {
    _source += code;
    _source += "\n";
}

void WGSL::endEntry() {
    _source += "}\n";
}

void WGSL::end() {
    _currentStage = wgpu::ShaderStage::None;
    _outputTypeName = "";
    _inputTypeName.clear();
}

//MARK: - Builder
WGSL::Builder& WGSL::Builder::begin(wgpu::ShaderStage stage) {
    _currentStage = stage;
    return *this;
}

WGSL::Builder& WGSL::Builder::addStruct(const std::string& code) {
    _source += code;
    return *this;
}

WGSL::Builder& WGSL::Builder::addUniformBinding(const std::string& name, UniformType type, uint32_t group) {
    return addUniformBinding(name, uniformTypeToString(type), group);
}

WGSL::Builder& WGSL::Builder::addUniformBinding(const std::string& name, const std::string& type, uint32_t group) {
    const std::string formatTemplate = "@group({}) @binding({})\n "
    "var<uniform> {}: {};\n ";
    
    auto property = Shader::getPropertyByName(name);
    if (property.has_value()) {
        uint32_t binding = property.value().uniqueId;
        _source += fmt::format(formatTemplate, group, binding,
                               name, type);
        
        wgpu::BindGroupLayoutEntry entry;
        entry.binding = binding;
        entry.visibility = _currentStage;
        entry.buffer.type = wgpu::BufferBindingType::Uniform;
        auto iter = _bindGroupLayoutEntryVecMap.find(group);
        if (iter == _bindGroupLayoutEntryVecMap.end()) {
            _bindGroupLayoutEntryVecMap[group] = {entry};
        } else {
            iter->second.push_back(entry);
        }
    } else {
        assert(false && "Unknown Uniform Name");
    }
    
    return *this;
}

WGSL::Builder& WGSL::Builder::beginInputStruct(const std::string& name) {
    _source += "struct ";
    _source += name;
    _source += " {\n";
    _inputTypeName.push_back(name);
    return *this;
}

WGSL::Builder& WGSL::Builder::addInputType(const std::string& code) {
    _source += code;
    _source += "\n";
    return *this;
}

WGSL::Builder& WGSL::Builder::endInputStruct() {
    _source += "}\n";
    return *this;
}

WGSL::Builder& WGSL::Builder::beginOutputStruct(const std::string& name) {
    _source += "struct ";
    _source += name;
    _source += " {\n";
    _outputTypeName = name;
    return *this;
}

WGSL::Builder& WGSL::Builder::addOutputType(const std::string& code) {
    _source += code;
    _source += "\n";
    return *this;
}

WGSL::Builder& WGSL::Builder::endOutputStruct() {
    _source += "}\n";
    return *this;
}

WGSL::Builder& WGSL::Builder::beginEntry(const std::initializer_list<std::string>& parameter) {
    if (_currentStage == wgpu::ShaderStage::Vertex) {
        _source += "@stage(vertex)\n";
    } else if (_currentStage == wgpu::ShaderStage::Fragment) {
        _source += "@stage(fragment)\n";
    } else {
        assert(false && "Use Begin at first");
    }
    
    auto inputIter = _inputTypeName.begin();
    _source += "fn main(";
    for (const auto& name : parameter) {
        if (inputIter != _inputTypeName.end()) {
            _source += name;
            _source += ": ";
            _source += *inputIter;
            _source += ", ";
            inputIter++;
        }
    }
    _source += ") -> ";
    _source += _outputTypeName;
    _source += " {\n";
    
    return *this;
}

WGSL::Builder& WGSL::Builder::addEntry(const std::string& code) {
    _source += code;
    _source += "\n";
    return *this;
}

WGSL::Builder& WGSL::Builder::endEntry() {
    _source += "}\n";
    return *this;
}

WGSL::Builder& WGSL::Builder::end() {
    _currentStage = wgpu::ShaderStage::None;
    _outputTypeName = "";
    _inputTypeName.clear();
    return *this;
}

WGSLPtr WGSL::Builder::build() {
    WGSLPtr source;
    if (_currentStage == wgpu::ShaderStage::None) {
        source = std::make_unique<WGSL>(_source, _bindGroupLayoutEntryVecMap);
    } else {
        source = nullptr;
    }
    _source = "";
    _bindGroupLayoutEntryVecMap.clear();
    
    return source;
}

}
