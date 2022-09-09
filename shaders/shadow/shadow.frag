#ifdef SHADOW_MAP_COUNT
layout(set = 0, binding = 25) uniform sampler2DArrayShadow shadowMap;

struct ShadowData {
    float bias;
    float intensity;
    float radius;
    float dump;
    mat4 vp[4];
    vec4 cascadeSplits;
};

layout(set = 0, binding = 26) uniform shadowData {
    ShadowData value[10];
} shadow_data;

const vec2 offsets[4] = vec2[](
    vec2(0, 0),
    vec2(0.5, 0),
    vec2(0, 0.5),
    vec2(0.5, 0.5)
);

float textureProj(vec3 worldPos, vec3 viewPos, vec2 off, int index) {
    // Get cascade index for the current fragment's view position
    uint cascadeIndex = 0;
    float scale = 1.0;
    if (shadow_data.value[index].cascadeSplits[0] * shadow_data.value[index].cascadeSplits[1] > 0) {
        scale = 0.5;
        for (uint i = 0; i < 4 - 1; ++i) {
            if (viewPos.z < shadow_data.value[index].cascadeSplits[i]) {
                cascadeIndex = i + 1;
            }
        }
    }

    vec4 shadowCoord = shadow_data.value[index].vp[cascadeIndex] * vec4(worldPos, 1.0);
    vec2 xy = shadowCoord.xy;
    xy /= shadowCoord.w;
    xy = xy * 0.5 + 0.5;
    xy *= scale;
    float shadow_sample = texture(shadowMap, vec4(xy + off + offsets[cascadeIndex], index, shadowCoord.z / shadowCoord.w));

    if (shadow_sample < 1.0) {
        return shadow_data.value[index].intensity;
    } else {
        return 1.0;
    }
}

float filterPCF(vec3 worldPos, vec3 viewPos, int index) {
    // Get cascade index for the current fragment's view position
    uint cascadeIndex = 0;
    float scale = 1.0;
    if (shadow_data.value[index].cascadeSplits[0] * shadow_data.value[index].cascadeSplits[1] > 0) {
        scale = 0.5;
        for (uint i = 0; i < 4 - 1; ++i) {
            if (viewPos.z < shadow_data.value[index].cascadeSplits[i]) {
                cascadeIndex = i + 1;
            }
        }
    }

    vec4 shadowCoord = shadow_data.value[index].vp[cascadeIndex] * vec4(worldPos, 1.0);
    vec2 xy = shadowCoord.xy;
    xy /= shadowCoord.w;
    xy = xy * 0.5 + 0.5;
    xy *= scale;

    const int neighborWidth = 3;
    const float neighbors = (neighborWidth * 2.0 + 1.0) * (neighborWidth * 2.0 + 1.0);
    float mapSize = 4096;
    float texelSize = 1.0 / mapSize;
    float total = 0.0;
    for (int x = -neighborWidth; x <= neighborWidth; x++) {
        for (int y = -neighborWidth; y <= neighborWidth; y++) {
            float shadow_sample = texture(shadowMap, vec4(xy + vec2(x, y) * texelSize + offsets[cascadeIndex], index, shadowCoord.z / shadowCoord.w));
            if (shadow_sample < 1.0) {
                total += shadow_data.value[index].intensity;
            } else {
                total += 1.0;
            }
        }
    }
    return total / neighbors;
}
#endif