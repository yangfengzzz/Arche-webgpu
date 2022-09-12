#version 450

layout(set = 0, binding = 0) uniform textureCube u_cubeTexture;
layout(set = 0, binding = 1) uniform sampler u_cubeSampler;

layout(location = 0) in vec3 v_cubeUV;
layout(location = 0) out vec4 o_color;

void main() {
    o_color = texture(samplerCube(u_cubeTexture, u_cubeSampler), v_cubeUV);
}