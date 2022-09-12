layout(set = 0, binding = Camera_Location) uniform u_cameraData {
    mat4 u_viewMat;
    mat4 u_projMat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 u_cameraPos;
};

layout(set = 0, binding = Renderer_Location) uniform u_rendererData {
    mat4 u_localMat;
    mat4 u_modelMat;
    mat4 u_normalMat;
};