#version 450

layout(set = 0, binding = 11) uniform samplerCube u_cube;
layout(location = 0) in vec3 v_cubeUV;
layout(location = 0) out vec4 o_color;

void main() {
    o_color = texture(u_cube, v_cubeUV);
}