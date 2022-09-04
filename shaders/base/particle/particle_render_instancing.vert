#version 450

#include "base/particle/particle_render_common.comp"
#include "base/particle/particle_config.h"

const vec2 pos[4] = vec2[](
    vec2(-1.0, 1.0),
    vec2(-1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(1.0, -1.0)
);

layout(set = 0, binding = 25) uniform cameraData {
    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 camera_pos;
};

layout(binding = 10)
readonly buffer writeConsumeBuffer {
    TParticle write_particles[];
};

layout(location = 0) in vec3 POSITION;

layout (location = 0) out vec2 uv;
layout (location = 1) out vec3 color;
layout (location = 2) out float decay;

void main() {
    // Time alived in [0, 1].
    const float dAge = 1.0f - maprange(0.0f, write_particles[gl_InstanceIndex].start_age, write_particles[gl_InstanceIndex].age);
    decay = curve_inout(dAge, 0.55f);

    uv = pos[gl_VertexIndex];
    vec4 worldPosApprox = proj_mat * view_mat * vec4(POSITION.xyz, 1.0);
    vec3 worldPos = vec3(uv, 0.0) * compute_size(worldPosApprox.z/worldPosApprox.w, decay) * 0.025; // todo

    // Generate a billboarded model view matrix
    mat4 bbModelViewMatrix = mat4(1.0);
    bbModelViewMatrix[3] = vec4(write_particles[gl_InstanceIndex].position.xyz, 1.0);
    bbModelViewMatrix = view_mat * bbModelViewMatrix;
    bbModelViewMatrix[0][0] = 1.0;
    bbModelViewMatrix[0][1] = 0.0;
    bbModelViewMatrix[0][2] = 0.0;

    bbModelViewMatrix[1][0] = 0.0;
    bbModelViewMatrix[1][1] = 1.0;
    bbModelViewMatrix[1][2] = 0.0;

    bbModelViewMatrix[2][0] = 0.0;
    bbModelViewMatrix[2][1] = 0.0;
    bbModelViewMatrix[2][2] = 1.0;

    gl_Position = proj_mat * bbModelViewMatrix * vec4(worldPos, 1.0);
    color = base_color(write_particles[gl_InstanceIndex].position.xyz, decay);
}