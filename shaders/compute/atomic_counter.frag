#version 450
#define Frag_Shader
#include "common.h"
#include "snippet/uv_define.h"

layout(location = 0) out vec4 o_color;
layout(binding = 50) buffer u_atomicCounter {
    uint atomic_counter;
};

void main() {
    uint atomic = atomic_counter % 255;
    o_color = vec4(atomic/255.0, 1 - atomic/255.0, atomic/255.0, 1.0);
}
