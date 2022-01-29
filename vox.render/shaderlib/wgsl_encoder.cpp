//
//  wgsl_encoder.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "wgsl_encoder.h"
#include "wgsl.h"
#include "shader/shader.h"
#include <fmt/core.h>

namespace vox {
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

void WGSLEncoder::addInputType(const std::string& structName, const std::string& code) {
    _inputType[structName].push_back(code);
    _needFlush = true;
}

void WGSLEncoder::addOutputType(const std::string& structName, const std::string& code) {
    if (!_outputType.first.empty() && _outputType.first != structName) {
        assert(false && "Output Type should be unique!");
    }
    _outputType.first = structName;
    _outputType.second.push_back(code);
    _needFlush = true;
}

void WGSLEncoder::addEntry(const std::initializer_list<std::string>& parameter,
                           std::function<std::string()> code) {
    if (_currentStage == wgpu::ShaderStage::Vertex) {
        _entryBlock += "@stage(vertex)\n";
    } else if (_currentStage == wgpu::ShaderStage::Fragment) {
        _entryBlock += "@stage(fragment)\n";
    } else {
        assert(false && "Use Begin at first");
    }
    
    auto inputIter = _inputType.begin();
    _entryBlock += "fn main(";
    for (const auto& name : parameter) {
        if (inputIter != _inputType.end()) {
            _entryBlock += name;
            _entryBlock += ": ";
            _entryBlock += inputIter->first;
            _entryBlock += ", ";
            inputIter++;
        }
    }
    _entryBlock += ") -> ";
    _entryBlock += _outputType.first;
    _entryBlock += " {\n";
    code();
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
    _outputType.first.clear();
    _outputType.second.clear();
    _inputType.clear();
    _entryBlock.clear();
    _needFlush = false;
}

void WGSLEncoder::_buildSource() {
    _source.clear();
    _source += _structBlock;
    _source += _uniformBlock;
    _source += _functionBlock;
    // Output
    {
        _source += "struct ";
        _source += _outputType.first;
        _source += " {\n";
        for (const auto& typeInfo : _outputType.second) {
            _source += typeInfo;
            _source += "\n";
        }
        _source += "}\n";
    }
    // Output
    {
        for (const auto& structInfo : _inputType) {
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
