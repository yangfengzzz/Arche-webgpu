#version 450

float LinearizeDepth(float depth) {
    float n = 1.0;// camera z near
    float f = 128.0;// camera z far
    float z = depth;
    return (2.0 * n) / (f + n - z * (f - n));
}

struct ShadowData {
    float bias;
    float intensity;
    float radius;
    float dump;
    mat4 vp[4];
    vec4 cascadeSplits;
};

layout(set = 0, binding = 23) uniform shadowData {
    ShadowData value[10];
} shadow_data;
layout(set = 0, binding = 22) uniform sampler2DArray shadowMap;

//----------------------------------------------------------------------------------------------------------------------
layout (location = 0) in vec2 v_uv;
layout (location = 5) in vec3 view_pos;
layout(location = 0) out vec4 o_color;

void main() {
    // Get cascade index for the current fragment's view position
    uint cascadeIndex = 0;
    for (uint i = 0; i < 4 - 1; ++i) {
        if (view_pos.z < shadow_data.value[0].cascadeSplits[i]) {
            cascadeIndex = i + 1;
        }
    }

    float depth = texture(shadowMap, vec3(v_uv, 0)).r;
    if (cascadeIndex == 0) {
        o_color = vec4(1, 1, 1, 1);
    } else if (cascadeIndex == 1) {
        o_color = vec4(1, 0, 0, 1);
    } else if (cascadeIndex == 2) {
        o_color = vec4(0, 1, 0, 1);
    } else if (cascadeIndex == 3) {
        o_color = vec4(0, 0, 1, 1);
    } else {
        o_color = vec4(vec3(1.0-LinearizeDepth(depth)), 1.0);
    }
}