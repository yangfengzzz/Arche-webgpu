//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
// int have no verb, other will use:
// HAS_ : Resouce
// OMMIT_ : Omit Resouce
// NEED_ : Shader Operation
// IS_ : Shader control flow
// _COUNT: type int constant
enum MacroName {
    HAS_UV = 0,
    HAS_NORMAL,
    HAS_TANGENT,
    HAS_VERTEXCOLOR,

    // Blend Shape
    HAS_BLENDSHAPE,
    HAS_BLENDSHAPE_TEXTURE,
    HAS_BLENDSHAPE_NORMAL,
    HAS_BLENDSHAPE_TANGENT,

    // Skin
    HAS_SKIN,
    HAS_JOINT_TEXTURE,
    JOINTS_COUNT,

    // Material
    NEED_ALPHA_CUTOFF,
    NEED_WORLDPOS,
    NEED_TILINGOFFSET,
    HAS_DIFFUSE_TEXTURE,
    HAS_SPECULAR_TEXTURE,
    HAS_EMISSIVE_TEXTURE,
    HAS_NORMAL_TEXTURE,
    OMIT_NORMAL,
    HAS_BASE_TEXTURE,
    HAS_BASE_COLORMAP,
    HAS_EMISSIVEMAP,
    HAS_OCCLUSIONMAP,
    HAS_SPECULARGLOSSINESSMAP,
    HAS_METALROUGHNESSMAP,

    // Light
    DIRECT_LIGHT_COUNT,
    POINT_LIGHT_COUNT,
    SPOT_LIGHT_COUNT,

    // Enviroment
    HAS_SH,
    HAS_SPECULAR_ENV,
    HAS_DIFFUSE_ENV,

    // Particle Render
    NEED_PARTICLE_SCATTERING,
    NEED_PARTICLE_VECTOR_FIELD,
    NEED_PARTICLE_CURL_NOISE,
    NEED_PARTICLE_VELOCITY_CONTROL,
    PARTICLE_COUNT,

    // Shadow
    SHADOW_MAP_COUNT,
    CUBE_SHADOW_MAP_COUNT,

    TOTAL_COUNT
};

}  // namespace vox