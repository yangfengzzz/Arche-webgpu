#version 450

#include "common.h"

layout(location = 0) in vec3 POSITION;
layout(location = 1) in vec3 COLOR;

layout(set = 0, binding = 10) uniform u_mvpNoscale {
    mat4 mvp_no_scale;
};

layout (location = 0) out vec4 COLOR0;

void main() {
    gl_Position = mvp_no_scale * vec4(POSITION, 1.0);
    COLOR0 = vec4(COLOR, 1.0);
}