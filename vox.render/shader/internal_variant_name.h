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
const std::string HAS_UV = "HAS_UV";
const std::string HAS_NORMAL = "HAS_NORMAL";
const std::string HAS_TANGENT = "HAS_TANGENT";
const std::string HAS_VERTEXCOLOR = "HAS_VERTEXCOLOR";

// Blend Shape
const std::string HAS_BLENDSHAPE = "HAS_BLENDSHAPE";
const std::string HAS_BLENDSHAPE_TEXTURE = "HAS_BLENDSHAPE_TEXTURE";
const std::string HAS_BLENDSHAPE_NORMAL = "HAS_BLENDSHAPE_NORMAL";
const std::string HAS_BLENDSHAPE_TANGENT = "HAS_BLENDSHAPE_TANGENT";

// Skin
const std::string HAS_SKIN = "HAS_SKIN";
const std::string HAS_JOINT_TEXTURE = "HAS_JOINT_TEXTURE";
const std::string JOINTS_COUNT = "JOINTS_COUNT ";

// Material
const std::string NEED_ALPHA_CUTOFF = "NEED_ALPHA_CUTOFF";
const std::string NEED_WORLDPOS = "NEED_WORLDPOS";
const std::string NEED_TILINGOFFSET = "NEED_TILINGOFFSET";
const std::string HAS_DIFFUSE_TEXTURE = "HAS_DIFFUSE_TEXTURE";
const std::string HAS_SPECULAR_TEXTURE = "HAS_SPECULAR_TEXTURE";
const std::string HAS_EMISSIVE_TEXTURE = "HAS_EMISSIVE_TEXTURE";
const std::string HAS_NORMAL_TEXTURE = "HAS_NORMAL_TEXTURE";
const std::string OMIT_NORMAL = "OMIT_NORMAL";
const std::string HAS_BASE_TEXTURE = "HAS_BASE_TEXTURE";
const std::string HAS_BASE_COLORMAP = "HAS_BASE_COLORMAP";
const std::string HAS_EMISSIVEMAP = "HAS_EMISSIVEMAP";
const std::string HAS_OCCLUSIONMAP = "HAS_OCCLUSIONMAP";
const std::string HAS_SPECULARGLOSSINESSMAP = "HAS_SPECULARGLOSSINESSMAP";
const std::string HAS_METALROUGHNESSMAP = "HAS_METALROUGHNESSMAP";

// Light
const std::string DIRECT_LIGHT_COUNT = "DIRECT_LIGHT_COUNT ";
const std::string POINT_LIGHT_COUNT = "POINT_LIGHT_COUNT ";
const std::string SPOT_LIGHT_COUNT = "SPOT_LIGHT_COUNT ";

// Enviroment
const std::string HAS_SH = "HAS_SH";
const std::string HAS_SPECULAR_ENV = "HAS_SPECULAR_ENV";
const std::string HAS_DIFFUSE_ENV = "HAS_DIFFUSE_ENV";

// Particle Render
const std::string NEED_PARTICLE_SCATTERING = "NEED_PARTICLE_SCATTERING";
const std::string NEED_PARTICLE_VECTOR_FIELD = "NEED_PARTICLE_VECTOR_FIELD";
const std::string NEED_PARTICLE_CURL_NOISE = "NEED_PARTICLE_CURL_NOISE";
const std::string NEED_PARTICLE_VELOCITY_CONTROL = "NEED_PARTICLE_VELOCITY_CONTROL";
const std::string PARTICLE_COUNT = "PARTICLE_COUNT ";

// Shadow
const std::string SHADOW_MAP_COUNT = "SHADOW_MAP_COUNT ";
const std::string CUBE_SHADOW_MAP_COUNT = "CUBE_SHADOW_MAP_COUNT ";

}  // namespace vox
