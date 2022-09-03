//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader.h"
#include "vox.base/logging.h"

namespace vox {
std::unordered_map<std::string, std::unique_ptr<Shader>> Shader::_shaderMap = {};
std::unordered_map<std::string, ShaderProperty> Shader::_propertyNameMap = {};
std::unordered_map<uint32_t, ShaderDataGroup> Shader::_propertyGroupMap = {};

Shader::Shader(const std::string &name,
               WGSLPtr&& vertexSource,
               WGSLPtr&& fragmentSource) :
name(name),
_vertexSource(std::move(vertexSource)),
_fragmentSource(std::move(fragmentSource)) {
}

const std::string& Shader::vertexSource(const ShaderMacroCollection& macros) {
    auto result = _vertexSource->compile(macros);
    for (const auto& info : result.second) {
        _bindGroupInfo[info.first].insert(info.second.begin(), info.second.end());
    }
    _updateFlag.first = true;
    
    return result.first;
}

const std::string& Shader::fragmentSource(const ShaderMacroCollection& macros) {
    auto result = _fragmentSource->compile(macros);
    for (const auto& info : result.second) {
        _bindGroupInfo[info.first].insert(info.second.begin(), info.second.end());
    }
    _updateFlag.second = true;
    
    return result.first;
}

const Shader::BindGroupLayoutDescriptorMap& Shader::bindGroupLayoutDescriptors(const ShaderMacroCollection& macros) {
    if (!_updateFlag.first && _vertexSource) {
        vertexSource(macros);
    }
    if (!_updateFlag.second && _fragmentSource) {
        fragmentSource(macros);
    }
    _updateFlag = {false, false};
    
    for (const auto& info : _bindGroupInfo) {
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
    return _bindGroupLayoutDescriptorMap;
}

void Shader::flush() {
    _bindGroupInfo.clear();
    _bindGroupLayoutEntryVecMap.clear();
    _bindGroupLayoutDescriptorMap.clear();
}

wgpu::BindGroupLayoutEntry Shader::_findEntry(uint32_t group, uint32_t binding) {
    wgpu::BindGroupLayoutEntry entry;
    entry.visibility = wgpu::ShaderStage::None;
    
    const auto& entryMap = _vertexSource->bindGroupLayoutEntryMap();
    auto iter = entryMap.find(group);
    if (iter != entryMap.end()) {
        auto entryIter = iter->second.find(binding);
        if (entryIter != iter->second.end()) {
            entry = entryIter->second;
        }
    }
    
    if (_fragmentSource) {
        const auto& entryMap = _fragmentSource->bindGroupLayoutEntryMap();
        auto iter = entryMap.find(group);
        if (iter != entryMap.end()) {
            auto entryIter = iter->second.find(binding);
            if (entryIter != iter->second.end()) {
                if (entry.visibility != wgpu::ShaderStage::None) {
                    entry.visibility |= wgpu::ShaderStage::Fragment;
                } else {
                    entry = entryIter->second;
                }
            }
        }
    }
    
    if (entry.visibility != wgpu::ShaderStage::None) {
        return entry;
    } else {
        assert(false);
        throw std::exception();
    }
}

Shader *Shader::create(const std::string &name,
                       WGSLPtr&& vertexSource,
                       WGSLPtr&& fragmentSource) {
    auto iter = Shader::_shaderMap.find(name);
    
    if (iter != Shader::_shaderMap.end()) {
        LOGE("Shader named {} already exists.", name)
    }
    auto shader = std::make_unique<Shader>(name, std::move(vertexSource), std::move(fragmentSource));
    auto shaderPtr = shader.get();
    Shader::_shaderMap.insert(std::make_pair(name, std::move(shader)));
    return shaderPtr;
}

Shader *Shader::find(const std::string &name) {
    auto iter = Shader::_shaderMap.find(name);
    if (iter != Shader::_shaderMap.end()) {
        return iter->second.get();
    } else {
        return nullptr;
    }
}

//MARK: - Property
std::optional<ShaderProperty> Shader::getPropertyByName(const std::string &name) {
    auto iter = Shader::_propertyNameMap.find(name);
    if (iter != Shader::_propertyNameMap.end()) {
        return iter->second;
    } else {
        return std::nullopt;
    }
}

std::optional<ShaderDataGroup> Shader::getShaderPropertyGroup(const std::string &propertyName) {
    auto iter = Shader::_propertyNameMap.find(propertyName);
    if (iter != Shader::_propertyNameMap.end()) {
        return iter->second.group;
    } else {
        return std::nullopt;
    }
}

std::optional<ShaderDataGroup> Shader::getShaderPropertyGroup(uint32_t uniqueID) {
    auto iter = Shader::_propertyGroupMap.find(uniqueID);
    if (iter != Shader::_propertyGroupMap.end()) {
        return iter->second;
    } else {
        return std::nullopt;
    }
}

ShaderProperty Shader::createProperty(const std::string &name, ShaderDataGroup group) {
    auto iter = Shader::_propertyNameMap.find(name);
    if (iter != Shader::_propertyNameMap.end()) {
        return iter->second;
    } else {
        auto property = ShaderProperty(name, group);
        Shader::_propertyNameMap.insert(std::make_pair(name, property));
        Shader::_propertyGroupMap[property.uniqueId] = group;
        return property;
    }
}

}
