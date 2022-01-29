//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_data.h"
#include "shader.h"

namespace vox {
ShaderData::ShaderData(wgpu::Device& device):
_device(device) {
}

std::optional<wgpu::Buffer> ShaderData::getData(const std::string &property_name) {
    auto property = Shader::getPropertyByName(property_name);
    if (property.has_value()) {
        return getData(property.value());
    } else {
        assert(false && "can't find property");
    }
}

std::optional<wgpu::Buffer> ShaderData::getData(const ShaderProperty &property) {
    auto iter = _properties.find(property.uniqueId);
    if (iter != _properties.end()) {
        return iter->second;
    } else {
        return std::nullopt;
    }
}

const std::unordered_map<int, wgpu::Buffer> &ShaderData::properties() const {
    return _properties;
}

//MARK: - Macro
void ShaderData::enableMacro(const std::string& macroName) {
    _macroCollection.enableMacro(macroName);
}

void ShaderData::enableMacro(const std::string& macroName, double value) {
    _macroCollection.enableMacro(macroName, value);
}

void ShaderData::disableMacro(const std::string& macroName) {
    _macroCollection.disableMacro(macroName);
}

void ShaderData::enableMacro(MacroName macroName) {
    _macroCollection.enableMacro(macroName);
}

void ShaderData::enableMacro(MacroName macroName, double value) {
    _macroCollection.enableMacro(macroName, value);
}

void ShaderData::disableMacro(MacroName macroName) {
    _macroCollection.disableMacro(macroName);
}

void ShaderData::mergeMacro(const ShaderMacroCollection &macros,
                            ShaderMacroCollection &result) const {
    ShaderMacroCollection::unionCollection(macros, _macroCollection, result);
}

}
