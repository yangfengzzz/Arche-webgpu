//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_data.h"
#include "shader.h"

namespace vox {
std::any ShaderData::getData(const std::string &property_name) {
    auto property = Shader::getPropertyByName(property_name);
    if (property.has_value()) {
        return getData(property.value());
    } else {
        assert(false && "can't find property");
    }
}

std::any ShaderData::getData(const ShaderProperty &property) {
    auto iter = _properties.find(property.uniqueId);
    if (iter != _properties.end()) {
        return iter->second;
    } else {
        return std::nullopt;
    }
}

void ShaderData::setData(const std::string &property_name, std::any value) {
    auto property = Shader::getPropertyByName(property_name);
    if (property.has_value()) {
        setData(property.value(), value);
    } else {
        assert(false && "can't find property");
    }
}

void ShaderData::setData(ShaderProperty property, std::any value) {
    _properties[property.uniqueId] = value;
}

const std::unordered_map<int, std::any> &ShaderData::properties() const {
    return _properties;
}

//MARK: - Macro
//void ShaderData::enableMacro(MacroName macroName) {
//    _macroCollection._value.insert(std::make_pair(macroName, std::make_pair(1, MTL::DataTypeBool)));
//}
//
//void ShaderData::enableMacro(MacroName macroName, std::pair<int, MTL::DataType> value) {
//    _macroCollection._value.insert(std::make_pair(macroName, value));
//}
//
//void ShaderData::disableMacro(MacroName macroName) {
//    auto iter = _macroCollection._value.find(macroName);
//    if (iter != _macroCollection._value.end()) {
//        _macroCollection._value.erase(iter);
//    }
//}
//
//void ShaderData::mergeMacro(const ShaderMacroCollection &macros,
//                            ShaderMacroCollection &result) const {
//    ShaderMacroCollection::unionCollection(macros, _macroCollection, result);
//}

}