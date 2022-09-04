#version 450

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 o_color;
layout(set = 0, binding = 36) uniform atomicCounter {
    uint value;
} atomic_counter;

void main() {
    uint atomic = atomic_counter.value % 255;
    o_color = vec4(atomic/255.0, 1 - atomic/255.0, atomic/255.0, 1.0);
}
