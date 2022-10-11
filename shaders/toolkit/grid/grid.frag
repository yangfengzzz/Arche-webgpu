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

layout(location = 1) in vec3 nearPoint;
layout(location = 2) in vec3 farPoint;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 20) uniform u_grid {
    float u_far;
    float u_near;
    float u_primaryScale;
    float u_secondaryScale;
    float u_gridIntensity;
    float u_axisIntensity;
    float u_flipProgress;
};

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = mix(fragPos3D.xz, fragPos3D.xy, u_flipProgress) * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1.0);
    float minimumx = min(derivative.x, 1.0);
    vec4 color = vec4(u_gridIntensity, u_gridIntensity, u_gridIntensity, 1.0 - min(line, 1.0));
    // z-axis
    if (fragPos3D.x > -u_axisIntensity * minimumx && fragPos3D.x < u_axisIntensity * minimumx)
        color.z = 1.0;
    // x-axis or y-axis
    float xy = mix(fragPos3D.z, fragPos3D.y, u_flipProgress);
    if (xy > -u_axisIntensity * minimumz && xy < u_axisIntensity * minimumz)
        color.x = 1.0;
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = u_projMat * u_viewMat * vec4(pos.xyz, 1.0);
    // map to 0-1
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = u_projMat * u_viewMat * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;// put back between -1 and 1
    float linearDepth = (2.0 * u_near * u_far) / (u_far + u_near - clip_space_depth * (u_far - u_near));
    return linearDepth / u_far;// normalize
}

void main() {
    float ty = -nearPoint.y / (farPoint.y - nearPoint.y);
    float tz = -nearPoint.z / (farPoint.z - nearPoint.z);
    float t = mix(ty, tz, u_flipProgress);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0.0, (0.5 - linearDepth));

    // adding multiple resolution for the grid
    outColor = (grid(fragPos3D, u_primaryScale, true) + grid(fragPos3D, u_secondaryScale, true));
    outColor.a *= fading;
}