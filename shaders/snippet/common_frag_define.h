layout(set = 0, binding = 5) uniform u_cameraData {
    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 camera_pos;
};

layout(set = 0, binding = 6) uniform u_rendererData {
    mat4 local_mat;
    mat4 model_mat;
    mat4 normal_mat;
};