//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shaderlib/wgsl_encoder.h"
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
    return _counters.size()-1;
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
    addUniformBinding(uniformName, toString(type), group);
}

void WGSLEncoder::addUniformBinding(const std::string& uniformName,
                                    const std::string& type, uint32_t group) {
    auto property = Shader::getPropertyByName(uniformName);
    if (property.has_value()) {
        addUniformBinding(uniformName, type, property.value().uniqueId, group);
    } else {
        assert(false && "Unknown Uniform Name");
    }
}

void WGSLEncoder::addUniformBinding(const std::string& uniformName, const std::string& type,
                                    uint32_t binding, uint32_t group) {
    const std::string formatTemplate = "@group({}) @binding({})\n "
    "var<uniform> {}: {};\n ";
    
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
    _needFlush = true;
}

void WGSLEncoder::addSampledTextureBinding(const std::string& texName, TextureType texType,
                                           const std::string& samplerName, SamplerType samplerType,
                                           uint32_t group) {
    auto texProperty = Shader::getPropertyByName(texName);
    auto samplerProperty = Shader::getPropertyByName(samplerName);
    if (texProperty.has_value() && samplerProperty.has_value()) {
        uint32_t texBinding = texProperty.value().uniqueId;
        uint32_t samplerBinding = samplerProperty.value().uniqueId;
        addSampledTextureBinding(texName, texType, texBinding,
                                 samplerName, samplerType, samplerBinding, group);
    } else {
        assert(false && "Unknown Uniform Name");
    }
}

void WGSLEncoder::addSampledTextureBinding(const std::string& texName, TextureType texType, uint32_t texBinding,
                                           const std::string& samplerName, SamplerType samplerType, uint32_t samplerBinding,
                                           uint32_t group) {
    const std::string formatTemplate = "@group({}) @binding({}) var {}: {};\n "
    "@group({}) @binding({}) var {}: {};\n ";
    
    _uniformBlock += fmt::format(formatTemplate, group, texBinding, texName, toString(texType),
                                 group, samplerBinding, samplerName, toString(samplerType));
    // Texture
    {
        wgpu::BindGroupLayoutEntry entry;
        entry.binding = texBinding;
        entry.visibility = _currentStage;
        entry.texture.multisampled = isMultisampled(texType);
        entry.texture.viewDimension = viewDimension(texType);
        entry.texture.sampleType = sampleType(texType);
        auto iter = _bindGroupLayoutEntryMap.find(group);
        if (iter == _bindGroupLayoutEntryMap.end()) {
            _bindGroupLayoutEntryMap[group][texBinding] = entry;
        } else {
            auto entryIter = _bindGroupLayoutEntryMap[group].find(texBinding);
            if (entryIter == _bindGroupLayoutEntryMap[group].end()) {
                _bindGroupLayoutEntryMap[group][texBinding] = entry;
            }
        }
        _bindGroupInfo[group].insert(texBinding);
    }
    // Sampler
    {
        wgpu::BindGroupLayoutEntry entry;
        entry.binding = samplerBinding;
        entry.visibility = _currentStage;
        entry.sampler.type = bindingType(samplerType);
        auto iter = _bindGroupLayoutEntryMap.find(group);
        if (iter == _bindGroupLayoutEntryMap.end()) {
            _bindGroupLayoutEntryMap[group][samplerBinding] = entry;
        } else {
            auto entryIter = _bindGroupLayoutEntryMap[group].find(samplerBinding);
            if (entryIter == _bindGroupLayoutEntryMap[group].end()) {
                _bindGroupLayoutEntryMap[group][samplerBinding] = entry;
            }
        }
        _bindGroupInfo[group].insert(samplerBinding);
    }
    _needFlush = true;
}

//MARK: - Storage
void WGSLEncoder::addStorageBufferBinding(const std::string& bufferName, UniformType type,
                                          bool isRead, uint32_t group) {
    addStorageBufferBinding(bufferName, toString(type), isRead, group);
}

void WGSLEncoder::addStorageBufferBinding(const std::string& bufferName, const std::string& type,
                                          bool isRead, uint32_t group) {
    auto property = Shader::getPropertyByName(bufferName);
    if (property.has_value()) {
        addStorageBufferBinding(bufferName, type, isRead, property.value().uniqueId, group);
    } else {
        assert(false && "Unknown Buffer Name");
    }
}

void WGSLEncoder::addStorageBufferBinding(const std::string& bufferName, const std::string& type,
                                          bool isRead, uint32_t binding, uint32_t group) {
    const std::string formatTemplate = "@group({}) @binding({})\n "
    "var<storage, {}> {}: {};\n ";
    
    if (isRead) {
        _uniformBlock += fmt::format(formatTemplate, group, binding,
                                     "read", bufferName, type);
    } else {
        _uniformBlock += fmt::format(formatTemplate, group, binding,
                                     "read_write", bufferName, type);
    }
    
    wgpu::BindGroupLayoutEntry entry;
    entry.binding = binding;
    entry.visibility = _currentStage;
    if (isRead) {
        entry.buffer.type = wgpu::BufferBindingType::ReadOnlyStorage;
    } else {
        entry.buffer.type = wgpu::BufferBindingType::Storage;
    }
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
    _needFlush = true;
}

void WGSLEncoder::addStorageTextureBinding(const std::string& texName, StorageTextureType texType,
                                           wgpu::TextureFormat texelFormat, uint32_t group) {
    const std::string formatTemplate = "@group({}) @binding({})\n "
    "var {}: {}<{}, write>;\n ";
    
    auto property = Shader::getPropertyByName(texName);
    if (property.has_value()) {
        uint32_t binding = property.value().uniqueId;
        _uniformBlock += fmt::format(formatTemplate, group, binding, texName,
                                     toString(texType), toString(texelFormat));
        
        wgpu::BindGroupLayoutEntry entry;
        entry.binding = binding;
        entry.visibility = _currentStage;
        entry.storageTexture.format = texelFormat;
        entry.storageTexture.viewDimension = viewDimension(texType);
        entry.storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
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

//MARK: - Inout
void WGSLEncoder::addInoutType(const std::string& structName,
                               Attributes attributes, UniformType type) {
    addInoutType(structName, (uint32_t)attributes, toString(attributes), toString(type));
}

void WGSLEncoder::addInoutType(const std::string& structName, uint32_t location,
                               const std::string& attributes, UniformType type) {
    addInoutType(structName, location, attributes, toString(type));
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
    addInoutType(structName, builtin, attributes, toString(type));
}

void WGSLEncoder::addInoutType(const std::string& structName, BuiltInType builtin,
                               const std::string& attributes, const std::string& type) {
    const std::string formatTemplate = "@builtin({}) {}: {};";
    _inoutType[structName].push_back(fmt::format(formatTemplate, toString(builtin),
                                                 attributes,  type));
    _needFlush = true;
}

void WGSLEncoder::addEntry(const std::initializer_list<std::pair<std::string, std::string>>& inParam,
                           const std::pair<std::string, std::string>& outType, std::function<void(std::string&)> code) {
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
    _entryBlock += outType.second;
    _entryBlock += " {\n";
    
    std::string formatTemplate = "var {}:{};\n";
    _entryBlock += fmt::format(formatTemplate, outType.first, outType.second);
    
    code(_entryBlock);
    
    formatTemplate = "return {};\n";
    _entryBlock += fmt::format(formatTemplate, outType.first);
    _entryBlock += "}\n";
    
    _needFlush = true;
}

void WGSLEncoder::addEntry(const Vector3F& workgroupSize,
                           const std::initializer_list<std::pair<std::string, std::string>>& inParam,
                           const std::pair<std::string, std::string>& outType, std::function<void(std::string&)> code) {
    if (_currentStage == wgpu::ShaderStage::Compute) {
        _entryBlock += fmt::format("@stage(compute), @workgroup_size({}, {}, {})\n",
                                   workgroupSize.x, workgroupSize.y, workgroupSize.z);
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
    _entryBlock += outType.second;
    _entryBlock += " {\n";
    
    std::string formatTemplate = "var {}:{};\n";
    _entryBlock += fmt::format(formatTemplate, outType.first, outType.second);
    
    code(_entryBlock);
    
    formatTemplate = "return {};\n";
    _entryBlock += fmt::format(formatTemplate, outType.first);
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
    _source += _functionBlock;
    _source += _entryBlock;
}

WGSLEncoder::WGSLEncoder(WGSL* wgsl, wgpu::ShaderStage currentStage):
_wgsl(wgsl),
_currentStage(currentStage){
}

}
