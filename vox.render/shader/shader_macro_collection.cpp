//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_macro_collection.h"

#include "std_helpers.h"

namespace vox {
void ShaderMacroCollection::unionCollection(const ShaderMacroCollection& left,
                                            const ShaderMacroCollection& right,
                                            ShaderMacroCollection& result) {
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
    auto iter = _value.find(std::hash<std::string>{}(macro));
    if (iter != _value.end()) {
        return true;
    } else {
        return false;
    }
}

std::optional<double> ShaderMacroCollection::macroConstant(const std::string& macro) const {
    auto iter = _value.find(std::hash<std::string>{}(macro));
    if (iter != _value.end()) {
        return iter->second;
    } else {
        return std::nullopt;
    }
}

void ShaderMacroCollection::enableMacro(const std::string& macroName) {
    _value.insert(std::make_pair(std::hash<std::string>{}(macroName), 1));
}

void ShaderMacroCollection::enableMacro(const std::string& macroName, double value) {
    _value.insert(std::make_pair(std::hash<std::string>{}(macroName), value));
}

void ShaderMacroCollection::disableMacro(const std::string& macroName) {
    auto iter = _value.find(std::hash<std::string>{}(macroName));
    if (iter != _value.end()) {
        _value.erase(iter);
    }
}

// MARK: - Internal Macro
std::vector<size_t> ShaderMacroCollection::_internalMacroHashValue = {
        std::hash<std::string>{}("HAS_UV"),
        std::hash<std::string>{}("HAS_NORMAL"),
        std::hash<std::string>{}("HAS_TANGENT"),
        std::hash<std::string>{}("HAS_VERTEXCOLOR"),

        // Blend Shape
        std::hash<std::string>{}("HAS_BLENDSHAPE"),
        std::hash<std::string>{}("HAS_BLENDSHAPE_TEXTURE"),
        std::hash<std::string>{}("HAS_BLENDSHAPE_NORMAL"),
        std::hash<std::string>{}("HAS_BLENDSHAPE_TANGENT"),

        // Skin
        std::hash<std::string>{}("HAS_SKIN"),
        std::hash<std::string>{}("HAS_JOINT_TEXTURE"),
        std::hash<std::string>{}("JOINTS_COUNT"),

        // Material
        std::hash<std::string>{}("NEED_ALPHA_CUTOFF"),
        std::hash<std::string>{}("NEED_WORLDPOS"),
        std::hash<std::string>{}("NEED_TILINGOFFSET"),
        std::hash<std::string>{}("HAS_DIFFUSE_TEXTURE"),
        std::hash<std::string>{}("HAS_SPECULAR_TEXTURE"),
        std::hash<std::string>{}("HAS_EMISSIVE_TEXTURE"),
        std::hash<std::string>{}("HAS_NORMAL_TEXTURE"),
        std::hash<std::string>{}("OMIT_NORMAL"),
        std::hash<std::string>{}("HAS_BASE_TEXTURE"),
        std::hash<std::string>{}("HAS_BASE_COLORMAP"),
        std::hash<std::string>{}("HAS_EMISSIVEMAP"),
        std::hash<std::string>{}("HAS_OCCLUSIONMAP"),
        std::hash<std::string>{}("HAS_SPECULARGLOSSINESSMAP"),
        std::hash<std::string>{}("HAS_METALROUGHNESSMAP"),

        // Light
        std::hash<std::string>{}("DIRECT_LIGHT_COUNT"),
        std::hash<std::string>{}("POINT_LIGHT_COUNT"),
        std::hash<std::string>{}("SPOT_LIGHT_COUNT"),

        // Enviroment
        std::hash<std::string>{}("HAS_SH"),
        std::hash<std::string>{}("HAS_SPECULAR_ENV"),
        std::hash<std::string>{}("HAS_DIFFUSE_ENV"),

        // Particle Render
        std::hash<std::string>{}("NEED_PARTICLE_SCATTERING"),
        std::hash<std::string>{}("NEED_PARTICLE_VECTOR_FIELD"),
        std::hash<std::string>{}("NEED_PARTICLE_CURL_NOISE"),
        std::hash<std::string>{}("NEED_PARTICLE_VELOCITY_CONTROL"),
        std::hash<std::string>{}("PARTICLE_COUNT"),

        // Shadow
        std::hash<std::string>{}("SHADOW_MAP_COUNT"),
        std::hash<std::string>{}("CUBE_SHADOW_MAP_COUNT"),
};

bool ShaderMacroCollection::contains(MacroName macro) const {
    auto iter = _value.find(_internalMacroHashValue[macro]);
    if (iter != _value.end()) {
        return true;
    } else {
        return false;
    }
}

std::optional<double> ShaderMacroCollection::macroConstant(MacroName macro) const {
    auto iter = _value.find(_internalMacroHashValue[macro]);
    if (iter != _value.end()) {
        return iter->second;
    } else {
        return std::nullopt;
    }
}

void ShaderMacroCollection::enableMacro(MacroName macroName) {
    _value.insert(std::make_pair(_internalMacroHashValue[macroName], 1));
}

void ShaderMacroCollection::enableMacro(MacroName macroName, double value) {
    _value.insert(std::make_pair(_internalMacroHashValue[macroName], value));
}

void ShaderMacroCollection::disableMacro(MacroName macroName) {
    auto iter = _value.find(_internalMacroHashValue[macroName]);
    if (iter != _value.end()) {
        _value.erase(iter);
    }
}

}  // namespace vox
