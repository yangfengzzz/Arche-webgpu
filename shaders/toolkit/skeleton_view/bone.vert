#version 450
#define Vert_Shader

mat4 GetWorldMatrix(mat4 joint) {
    // Rebuilds bone properties.
    // Bone length is set to zero to disable leaf rendering.
    float is_bone = joint[3].w;
    vec3 bone_dir = vec3(joint[0].w, joint[1].w, joint[2].w) * is_bone;
    float bone_len = length(bone_dir);
    
    // Setup rendering world matrix.
    float dot1 = dot(joint[2].xyz, bone_dir);
    float dot2 = dot(joint[0].xyz, bone_dir);
    vec3 binormal = abs(dot1) < abs(dot2) ? joint[2].xyz : joint[0].xyz;
    
    mat4 world_matrix;
    world_matrix[0] = vec4(bone_dir, 0.);
    world_matrix[1] = 
      vec4(bone_len * normalize(cross(binormal, bone_dir)), 0.);
    world_matrix[2] =
      vec4(bone_len * normalize(cross(bone_dir, world_matrix[1].xyz)), 0.);
    world_matrix[3] = vec4(joint[3].xyz, 1.);
    return world_matrix;
};

#include <snippet/common_vert_define.h>

layout(set = 0, binding = Joint_Matrix_Location) uniform u_jointMatrix {
    mat4 joint_matrix[4096];
};

layout(set = 0, binding = Base_Color_Location) uniform u_baseColor {
    vec4 base_color;
};

layout (location = 0) out vec3 v_world_normal;
layout (location = 1) out vec4 v_vertex_color;

void main() {
    mat4 world_matrix = GetWorldMatrix(u_jointMatrix[instance_id]);
    vec4 vertex = vec4(POSITION, 1.);
    gl_Position = vp_mat * u_modelMat * world_matrix * vertex;
    mat3 cross_matrix = mat3(
    cross(world_matrix[1].xyz, world_matrix[2].xyz),
    cross(world_matrix[2].xyz, world_matrix[0].xyz),
    cross(world_matrix[0].xyz, world_matrix[1].xyz));
    float invdet = 1.0 / dot(cross_matrix[2], world_matrix[2].xyz);
    mat3 normal_matrix = cross_matrix * invdet;
    v_world_normal = normal_matrix * NORMAL;
    v_vertex_color = base_color;
};