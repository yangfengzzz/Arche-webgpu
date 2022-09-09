#version 450

layout(location = 0) in vec3 POSITION;
layout(location = 1) in uint COLOR;

layout(set = 0, binding = 5) uniform cameraData {
    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 camera_pos;
};

layout(set = 0, binding = 6) uniform rendererData {
    mat4 local_mat;
    mat4 model_mat;
    mat4 normal_mat;
};

layout (location = 0) out uint v_color;

void main() {
    v_color = COLOR;
    vec4 position = vec4(POSITION, 1.0);
    gl_Position = vp_mat * model_mat * position;
}