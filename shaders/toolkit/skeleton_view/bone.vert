#version 450
#define Vert_Shader

#include "common.h"
#include "snippet/common_vert_define.h"

mat4 GetWorldMatrix(vec4 joint0, vec4 joint1, vec4 joint2, vec4 joint3) {
    // Rebuilds bone properties.
    // Bone length is set to zero to disable leaf rendering.
    float is_bone = joint3.w;
    vec3 bone_dir = vec3(joint0.w, joint1.w, joint2.w) * is_bone;
    float bone_len = length(bone_dir);
    
    // Setup rendering world matrix.
    float dot1 = dot(joint2.xyz, bone_dir);
    float dot2 = dot(joint0.xyz, bone_dir);
    vec3 binormal = abs(dot1) < abs(dot2) ? joint2.xyz : joint0.xyz;

    mat4 world_matrix;
    world_matrix[0] = vec4(bone_dir, 0.0);
    world_matrix[1] = vec4(bone_len * normalize(cross(binormal, bone_dir)), 0.0);
    world_matrix[2] = vec4(bone_len * normalize(cross(bone_dir, world_matrix[1].xyz)), 0.0);
    world_matrix[3] = vec4(joint3.xyz, 1.0);
    return world_matrix;
}

layout(location = 7) in vec4 joint0;
layout(location = 8) in vec4 joint1;
layout(location = 9) in vec4 joint2;
layout(location = 10) in vec4 joint3;

layout (location = 0) out vec3 v_world_normal;
layout (location = 1) out vec4 v_vertex_color;

void main() {
    mat4 world_matrix = GetWorldMatrix(joint0, joint1, joint2, joint3);
    vec4 vertex = vec4(POSITION, 1.);
    gl_Position = vp_mat * u_modelMat * world_matrix * vertex;
    mat3 cross_matrix = mat3(
    cross(world_matrix[1].xyz, world_matrix[2].xyz),
    cross(world_matrix[2].xyz, world_matrix[0].xyz),
    cross(world_matrix[0].xyz, world_matrix[1].xyz));
    float invdet = 1.0 / dot(cross_matrix[2], world_matrix[2].xyz);
    mat3 normal_matrix = cross_matrix * invdet;
    v_world_normal = normal_matrix * NORMAL;
    v_vertex_color = COLOR_0;
}