#version 450

#include "snippet/common_define.h"

layout(set = 0, binding = 1) uniform u_baseColor {
    vec4 base_color;
};
layout(set = 0, binding = 2) uniform u_alphaCutoff {
    float alpha_cutoff;
};

#ifdef HAS_BASE_TEXTURE
    layout(set = 0, binding = 3) uniform texture2D u_baseTexture;
    layout(set = 0, binding = 4) uniform sampler u_baseSampler;
#endif

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 o_color;

void main() {
    vec4 baseColor = base_color;
    
#ifdef HAS_BASE_TEXTURE
    vec4 textureColor = texture(sampler2D(u_baseTexture, u_baseSampler), v_uv);
    baseColor *= textureColor;
#endif
    
#ifdef NEED_ALPHA_CUTOFF
    if (base_color.a < alpha_cutoff) {
        discard;
    }
#endif
    
    o_color = baseColor;
}
