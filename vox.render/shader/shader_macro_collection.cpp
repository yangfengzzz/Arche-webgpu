//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_macro_collection.h"
#include "std_helpers.h"

namespace vox {
void ShaderMacroCollection::unionCollection(const ShaderMacroCollection &left, const ShaderMacroCollection &right,
                                            ShaderMacroCollection &result) {
    result._value.insert(left._value.begin(), left._value.end());
    result._value.insert(right._value.begin(), right._value.end());
}

size_t ShaderMacroCollection::hash() const {
    std::size_t hash{0U};
    for (const auto& value : _value) {
        hash_combine(hash, value.first);
        hash_combine(hash, value.second);
    }
    return hash;
}

bool ShaderMacroCollection::contains(const std::string& macro) const {
    auto iter = _value.find(macro);
    if (iter != _value.end()) {
        return true;
    } else {
        return false;
    }
}

std::optional<double> ShaderMacroCollection::macroConstant(const std::string& macro) const {
    auto iter = _value.find(macro);
    if (iter != _value.end()) {
        return iter->second;
    } else {
        return std::nullopt;
    }
}

void ShaderMacroCollection::enableMacro(const std::string& macroName) {
    _value.insert(std::make_pair(macroName, 1));
}

void ShaderMacroCollection::enableMacro(const std::string& macroName, double value) {
    _value.insert(std::make_pair(macroName, value));
}

void ShaderMacroCollection::disableMacro(const std::string& macroName) {
    auto iter = _value.find(macroName);
    if (iter != _value.end()) {
        _value.erase(iter);
    }
}

}
