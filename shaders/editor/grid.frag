#version 450

layout(location = 1) in vec3 nearPoint;
layout(location = 2) in vec3 farPoint;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 5) uniform cameraData {
    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 camera_pos;
};

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.6, 0.6, 0.6, 1.0 - min(line, 1.0));
    // z axis
    if (fragPos3D.x > -1 * minimumx && fragPos3D.x < 1 * minimumx)
        color = vec4(0.0, 0.0, 1.0, 1.0);
    // x axis
    if (fragPos3D.z > -1 * minimumz && fragPos3D.z < 1 * minimumz)
        color = vec4(1.0, 0.0, 0.0, 1.0);
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = proj_mat * view_mat * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

const float far = 50;
const float near = 0.01;

float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = proj_mat * view_mat * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;// put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near));// get linear value between 0.01 and 100
    return linearDepth / far;// normalize
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    outColor = grid(fragPos3D, 1, true) * float(t > 0);// adding multiple resolution for the grid
    outColor.a *= fading;
}