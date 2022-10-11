#version 450

#include "common.h"

layout(set = 0, binding = Camera_Location) uniform u_cameraData {
    mat4 u_viewMat;
    mat4 u_projMat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 u_cameraPos;
};

layout (location = 1) out vec3 nearPoint;
layout (location = 2) out vec3 farPoint;

// Grid position are in clipped space
vec3 gridPlane[6] = vec3[] (
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z) {
    vec4 unprojectedPoint =  view_inv_mat * proj_inv_mat * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 p = gridPlane[gl_VertexIndex].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0).xyz;// unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0).xyz;// unprojecting on the far plane
    gl_Position = vec4(p, 1.0);// using directly the clipped coordinates
}