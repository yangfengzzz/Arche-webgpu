//
//  internal_macro_name.h
//  DigitalArche
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef internal_macro_name_h
#define internal_macro_name_h

namespace vox {
// int have no verb, other will use:
// HAS_ : Resouce
// OMMIT_ : Omit Resouce
// NEED_ : Shader Operation
// IS_ : Shader control flow
// _COUNT: type int constant
enum MacroName {
    HAS_UV = 0,
    HAS_NORMAL = 1,
    HAS_TANGENT = 2,
    HAS_VERTEXCOLOR = 3,
    
    // Blend Shape
    HAS_BLENDSHAPE = 4,
    HAS_BLENDSHAPE_NORMAL = 5,
    HAS_BLENDSHAPE_TANGENT = 6,
    
    // Skin
    HAS_SKIN = 7,
    HAS_JOINT_TEXTURE = 8,
    JOINTS_COUNT = 9,
    
    // Material
    NEED_ALPHA_CUTOFF = 10,
    NEED_WORLDPOS = 11,
    NEED_TILINGOFFSET = 12,
    HAS_DIFFUSE_TEXTURE = 13,
    HAS_SPECULAR_TEXTURE = 14,
    HAS_EMISSIVE_TEXTURE = 15,
    HAS_NORMAL_TEXTURE = 16,
    OMIT_NORMAL = 17,
    HAS_BASE_TEXTURE = 18,
    HAS_BASE_COLORMAP = 19,
    HAS_EMISSIVEMAP = 20,
    HAS_OCCLUSIONMAP = 21,
    HAS_SPECULARGLOSSINESSMAP = 22,
    HAS_METALROUGHNESSMAP = 23,
    IS_METALLIC_WORKFLOW = 24,
    
    // Light
    DIRECT_LIGHT_COUNT = 25,
    POINT_LIGHT_COUNT = 26,
    SPOT_LIGHT_COUNT = 27,
    
    // Enviroment
    HAS_SH = 28,
    HAS_SPECULAR_ENV = 29,
    HAS_DIFFUSE_ENV = 30,
    
    // Particle Render
    HAS_PARTICLE_TEXTURE = 31,
    NEED_ROTATE_TO_VELOCITY = 32,
    NEED_USE_ORIGIN_COLOR = 33,
    NEED_SCALE_BY_LIFE_TIME = 34,
    NEED_FADE_IN = 35,
    NEED_FADE_OUT = 36,
    IS_2D = 37,
    
    // Shadow
    SHADOW_MAP_COUNT = 39,
    CUBE_SHADOW_MAP_COUNT = 40,

    TOTAL_COUNT = 41,
};

}

#endif /* internal_macro_name_h */
