//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_encoder.h"
#include "wgsl.h"
#include "shader/shader.h"
#include <fmt/core.h>

namespace vox {
std::vector<uint32_t> WGSLEncoder::_counters{};
size_t WGSLEncoder::startCounter(uint32_t initVal) {
    for (size_t i = 0; i < _counters.size(); i++) {
        if (_counters[i] == static_cast<uint32_t>(-1)) {
            _counters[i] = initVal;
            return i;
        }
    }
    _counters.push_back(initVal);
    return _counters.size();
}

uint32_t WGSLEncoder::getCounterNumber(size_t index) {
    return _counters[index]++;
}

void WGSLEncoder::endCounter(size_t index) {
    _counters[index] = static_cast<uint32_t>(-1);
}

WGSLEncoder::~WGSLEncoder() {
    if (_needFlush) {
        flush();
    }
}

void WGSLEncoder::addStruct(const std::string& code) {
    _structBlock += code;
    _needFlush = true;
}

void WGSLEncoder::addFunction(const std::string& code) {
    _functionBlock += code;
    _needFlush = true;
}

void WGSLEncoder::addUniformBinding(const std::string& uniformName,
                                    UniformType type, uint32_t group) {
    addUniformBinding(uniformName, uniformTypeToString(type), group);
    _needFlush = true;
}

void WGSLEncoder::addUniformBinding(const std::string& uniformName,
                                    const std::string& type, uint32_t group) {
    const std::string formatTemplate = "@group({}) @binding({})\n "
    "var<uniform> {}: {};\n ";
    
    auto property = Shader::getPropertyByName(uniformName);
    if (property.has_value()) {
        uint32_t binding = property.value().uniqueId;
        _uniformBlock += fmt::format(formatTemplate, group, binding,
                                     uniformName, type);
        
        wgpu::BindGroupLayoutEntry entry;
        entry.binding = binding;
        entry.visibility = _currentStage;
        entry.buffer.type = wgpu::BufferBindingType::Uniform;
        auto iter = _bindGroupLayoutEntryMap.find(group);
        if (iter == _bindGroupLayoutEntryMap.end()) {
            _bindGroupLayoutEntryMap[group][binding] = entry;
        } else {
            auto entryIter = _bindGroupLayoutEntryMap[group].find(binding);
            if (entryIter == _bindGroupLayoutEntryMap[group].end()) {
                _bindGroupLayoutEntryMap[group][binding] = entry;
            }
        }
        _bindGroupInfo[group].insert(binding);
    } else {
        assert(false && "Unknown Uniform Name");
    }
    _needFlush = true;
}

void WGSLEncoder::addInoutType(const std::string& structName,
                               Attributes attributes, UniformType type) {
    addInoutType(structName, (uint32_t)attributes, attributesToString(attributes), uniformTypeToString(type));
}

void WGSLEncoder::addInoutType(const std::string& structName, uint32_t location,
                               const std::string& attributes, UniformType type) {
    addInoutType(structName, location, attributes, uniformTypeToString(type));
}

void WGSLEncoder::addInoutType(const std::string& structName, uint32_t location,
                               const std::string& attributes, const std::string& type) {
    const std::string formatTemplate = "@location({}) {}: {};";
    _inoutType[structName].push_back(fmt::format(formatTemplate, location,
                                                 attributes,  type));
    _needFlush = true;
}

void WGSLEncoder::addInoutType(const std::string& structName, BuiltInType builtin,
                               const std::string& attributes, UniformType type) {
    addInoutType(structName, builtin, attributes, uniformTypeToString(type));
}

void WGSLEncoder::addInoutType(const std::string& structName, BuiltInType builtin,
                               const std::string& attributes, const std::string& type) {
    const std::string formatTemplate = "@builtin({}) {}: {};";
    _inoutType[structName].push_back(fmt::format(formatTemplate, builtInTypeToString(builtin),
                                                 attributes,  type));
    _needFlush = true;
}

void WGSLEncoder::addEntry(const std::initializer_list<std::pair<std::string, std::string>>& inParam,
                           const std::string& outType, std::function<void(std::string&)> code) {
    if (_currentStage == wgpu::ShaderStage::Vertex) {
        _entryBlock += "@stage(vertex)\n";
    } else if (_currentStage == wgpu::ShaderStage::Fragment) {
        _entryBlock += "@stage(fragment)\n";
    } else {
        assert(false && "Use Begin at first");
    }
    
    _entryBlock += "fn main(";
    for (const auto& param : inParam) {
        _entryBlock += param.first;
        _entryBlock += ": ";
        _entryBlock += param.second;
        _entryBlock += ", ";
    }
    _entryBlock += ") -> ";
    _entryBlock += outType;
    _entryBlock += " {\n";
    code(_entryBlock);
    _entryBlock += "}\n";
    
    _needFlush = true;
}

void WGSLEncoder::flush() {
    if (_needFlush) {
        _buildSource();
        _wgsl->_setSource(_source);
        _wgsl->_setBindGroupInfo(_bindGroupInfo);
        _wgsl->_setBindGroupLayoutEntryMap(_bindGroupLayoutEntryMap);
        clearCache();
    }
}

void WGSLEncoder::clearCache() {
    _structBlock.clear();
    _uniformBlock.clear();
    _functionBlock.clear();
    _inoutType.clear();
    _entryBlock.clear();
    _needFlush = false;
}

void WGSLEncoder::_buildSource() {
    _source.clear();
    _source += _structBlock;
    _source += _uniformBlock;
    _source += _functionBlock;
    // Inout
    {
        for (const auto& structInfo : _inoutType) {
            _source += "struct ";
            _source += structInfo.first;
            _source += " {\n";
            for (const auto& typeInfo : structInfo.second) {
                _source += typeInfo;
                _source += "\n";
            }
            _source += "}\n";
        }
    }
    _source += _entryBlock;
}

WGSLEncoder::WGSLEncoder(WGSL* wgsl, wgpu::ShaderStage currentStage):
_wgsl(wgsl),
_currentStage(currentStage){
}

}
