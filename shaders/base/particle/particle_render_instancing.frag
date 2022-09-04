#version 450

#include "base/particle/particle_render_common.comp"

layout (location = 0) in vec2 uv;
layout (location = 1) in vec3 color;
layout (location = 2) in float decay;
layout(location = 0) out vec4 o_color;

void main() {
    o_color = compute_color(color, decay, uv);
}