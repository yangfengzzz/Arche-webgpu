#version 450

#define PI 3.14159265359
#define RECIPROCAL_PI 0.31830988618
#define EPSILON 1e-6
#define LOG2 1.442695

#define saturate(a) clamp(a, 0.0, 1.0)
#define whiteCompliment(a) (1.0 - saturate(a))

vec4 RGBMToLinear(vec4 value, float maxRange) {
    return vec4(value.rgb * value.a * maxRange, 1.0);
}

vec4 gammaToLinear(vec4 srgbIn){
    return vec4(pow(srgbIn.rgb, vec3(2.2)), srgbIn.a);
}

vec4 linearToGamma(vec4 linearIn){
    return vec4(pow(linearIn.rgb, vec3(1.0 / 2.2)), linearIn.a);
}

layout(set = 0, binding = 1) uniform baseColor {
    vec4 value;
} base_color;
layout(set = 0, binding = 2) uniform alphaCutoff {
    float value;
} alpha_cutoff;

#ifdef HAS_BASE_TEXTURE
    layout(set = 0, binding = 3) uniform sampler2D baseTexture;
#endif

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 o_color;

void main() {
    vec4 baseColor = base_color.value;

    #ifdef HAS_BASE_TEXTURE
        vec4 textureColor = texture(baseTexture, v_uv);
        baseColor *= textureColor;
    #endif

    #ifdef NEED_ALPHA_CUTOFF
    if (base_color.value.a < alpha_cutoff.value) {
        discard;
    }
    #endif

    o_color = baseColor;
}