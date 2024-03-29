#version 450

#include "common.h"

layout(location = Position) in vec3 POSITION;
layout(location = UV_0) in vec2 TEXCOORD_0;

layout(set = 0, binding = 1) uniform u_cameraData {
    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 camera_pos;
};

layout(set = 0, binding = 2) uniform u_rendererData {
    mat4 local_mat;
    mat4 model_mat;
    mat4 normal_mat;
};

layout (location = 0) out vec2 v_uv;

void main(){
    gl_Position = vp_mat * model_mat * vec4(POSITION, 1.0);
    v_uv = TEXCOORD_0;
}