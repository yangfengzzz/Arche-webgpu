#version 450

#include "base/common.h"

layout(location = Position) in vec3 POSITION;

layout(set = 0, binding = 6) uniform rendererData {
    mat4 local_mat;
    mat4 model_mat;
    mat4 normal_mat;
} renderer_data;

layout(set = 0, binding = 10) uniform viewProjMat {
    mat4 value;
} view_proj;

void main() {
    vec4 position = vec4(POSITION, 1.0);
    gl_Position = view_proj.value * renderer_data.model_mat * position;
}