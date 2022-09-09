#version 450

const vec2 pos[4] = vec2[](
    vec2(-1.0, 1.0),
    vec2(-1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(1.0, -1.0)
);

layout(set = 0, binding = 5) uniform cameraData {
    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 camera_pos;
} camera_data;

// spot light
#ifdef IS_SPOT_LIGHT
    struct SpotLight {
        vec3 color;
        vec3 position;
        vec3 direction;
        float distance;
        float angleCos;
        float penumbraCos;
    };
    layout(set = 0, binding = 11) uniform spotLight {
        SpotLight value[SPOT_LIGHT_COUNT];
    } spot_light;
#else
    struct PointLight {
        vec3 color;
        vec3 position;
        float distance;
    };
    layout(set = 0, binding = 10) uniform pointLight {
        PointLight value[POINT_LIGHT_COUNT];
    } point_light;
#endif

layout (location = 0) out vec2 localPos;
layout (location = 1) out vec3 color;

void main() {
    localPos = pos[gl_VertexIndex];
    #ifdef IS_SPOT_LIGHT
        color = spot_light.value[gl_InstanceIndex].color;
        vec3 worldPos = vec3(localPos, 0.0) * spot_light.value[gl_InstanceIndex].distance * 0.025;
    #else
        color = point_light.value[gl_InstanceIndex].color;
        vec3 worldPos = vec3(localPos, 0.0) * point_light.value[gl_InstanceIndex].distance * 0.025;
    #endif

    // Generate a billboarded model view matrix
    mat4 bbModelViewMatrix = mat4(1.0);
    #ifdef IS_SPOT_LIGHT
        bbModelViewMatrix[3] = vec4(spot_light.value[gl_InstanceIndex].position, 1.0);
    #else
        bbModelViewMatrix[3] = vec4(point_light.value[gl_InstanceIndex].position, 1.0);
    #endif
    bbModelViewMatrix = camera_data.view_mat * bbModelViewMatrix;
    bbModelViewMatrix[0][0] = 1.0;
    bbModelViewMatrix[0][1] = 0.0;
    bbModelViewMatrix[0][2] = 0.0;

    bbModelViewMatrix[1][0] = 0.0;
    bbModelViewMatrix[1][1] = 1.0;
    bbModelViewMatrix[1][2] = 0.0;

    bbModelViewMatrix[2][0] = 0.0;
    bbModelViewMatrix[2][1] = 0.0;
    bbModelViewMatrix[2][2] = 1.0;

    gl_Position = camera_data.proj_mat * bbModelViewMatrix * vec4(worldPos, 1.0);
}