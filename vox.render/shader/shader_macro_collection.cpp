//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_macro_collection.h"
#include "std_helpers.h"

namespace vox {
std::unordered_map<std::string, double> ShaderMacroCollection::defaultValue = {
    {"HAS_UV", 0},
    {"HAS_NORMAL", 0},
    {"HAS_TANGENT", 0},
    {"HAS_VERTEXCOLOR", 0},
    
    // Blend Shape
    {"HAS_BLENDSHAPE", 0},
    {"HAS_BLENDSHAPE_NORMAL", 0},
    {"HAS_BLENDSHAPE_TANGENT", 0},
    
    // Skin
    {"HAS_SKIN", 0},
    {"HAS_JOINT_TEXTURE", 0},
    {"JOINTS_COUNT", 0},
    
    // Material
    {"NEED_ALPHA_CUTOFF", 0},
    {"NEED_WORLDPOS", 0},
    {"NEED_TILINGOFFSET", 0},
    {"HAS_DIFFUSE_TEXTURE", 0},
    {"HAS_SPECULAR_TEXTURE", 0},
    {"HAS_EMISSIVE_TEXTURE", 0},
    {"HAS_NORMAL_TEXTURE", 0},
    {"OMIT_NORMAL", 0},
    {"HAS_BASE_TEXTURE", 0},
    {"HAS_BASE_COLORMAP", 0},
    {"HAS_EMISSIVEMAP", 0},
    {"HAS_OCCLUSIONMAP", 0},
    {"HAS_SPECULARGLOSSINESSMAP", 0},
    {"HAS_METALROUGHNESSMAP", 0},
    {"IS_METALLIC_WORKFLOW", 0},
    
    // Light
    {"DIRECT_LIGHT_COUNT", 0},
    {"POINT_LIGHT_COUNT", 0},
    {"SPOT_LIGHT_COUNT", 0},
    
    // Enviroment
    {"HAS_SH", 0},
    {"HAS_SPECULAR_ENV", 0},
    {"HAS_DIFFUSE_ENV", 0},
    
    // Particle Render
    {"HAS_PARTICLE_TEXTURE", 0},
    {"NEED_ROTATE_TO_VELOCITY", 0},
    {"NEED_USE_ORIGIN_COLOR", 0},
    {"NEED_SCALE_BY_LIFE_TIME", 0},
    {"NEED_FADE_IN", 0},
    {"NEED_FADE_OUT", 0},
    {"IS_2D", 0},
    
    // Shadow
    {"SHADOW_MAP_COUNT", 0},
    {"CUBE_SHADOW_MAP_COUNT", 0},
};

std::vector<wgpu::ConstantEntry> ShaderMacroCollection::createDefaultFunction() {
    std::vector<wgpu::ConstantEntry> functionConstants(defaultValue.size());
    size_t i = 0;
    for (const auto& constant : defaultValue) {
        functionConstants[i].key = constant.first.c_str();
        functionConstants[i].value = constant.second;
    }
    return functionConstants;
}

void ShaderMacroCollection::unionCollection(const ShaderMacroCollection &left, const ShaderMacroCollection &right,
                                            ShaderMacroCollection &result) {
    result._value.insert(left._value.begin(), left._value.end());
    result._value.insert(right._value.begin(), right._value.end());
}

size_t ShaderMacroCollection::hash() const {
    std::size_t hash{0U};
    for (const auto& constant : defaultValue) {
        auto iter = _value.find(constant.first);
        if (iter != _value.end()) {
            hash_combine(hash, iter->first);
        }
    }
    return hash;
}


}
