#version 450
#define Vert_Shader

#include "common.h"
#include "snippet/common_vert_define.h"
#include "snippet/blendShape_define.h"
#include "snippet/normal_define.h"

layout(set = 0, binding = 51) uniform u_lightViewProjMat {
    mat4 light_view_proj_mat;
};

// x: depth bias, y: normal bias
layout(set = 0, binding = 52) uniform u_shadowBias {
    vec2 shadow_bias;
};

layout(set = 0, binding = 53) uniform u_lightDirection {
    vec3 light_direction;
};

vec3 applyShadowBias(vec3 positionWS) {
    positionWS -= light_direction * shadow_bias.x;
    return positionWS;
}

vec3 applyShadowNormalBias(vec3 positionWS, vec3 normalWS) {
    float invNdotL = 1.0 - clamp(dot(-light_direction, normalWS), 0.0, 1.0);
    float scale = invNdotL * shadow_bias.y;
    positionWS += normalWS * vec3(scale);
    return positionWS;
}

void main() {
#include "snippet/begin_position.vert"
#include "snippet/begin_normal.vert"
#include "snippet/blendShape.vert"
#include "snippet/skinning.vert"
#include "snippet/normal.vert"

    vec4 temp_pos = u_modelMat * position;
    vec3 v_pos = temp_pos.xyz / temp_pos.w;

    v_pos = applyShadowBias(v_pos);
    #ifndef OMIT_NORMAL
        #ifdef HAS_NORMAL
            v_pos = applyShadowNormalBias(v_pos, v_normal);
        #endif
    #endif

    gl_Position = light_view_proj_mat * vec4(v_pos, 1.0);

}
