#version 450
#define Frag_Shader

#include "common.h"
#include "snippet/common_define.h"

layout(set = 0, binding = Base_Color_Location) uniform u_baseColor {
    vec4 base_color;
};
layout(set = 0, binding = Alpha_Cutoff_Location) uniform u_alphaCutoff {
    float alpha_cutoff;
};

#ifdef HAS_BASE_TEXTURE
    layout(set = 0, binding = Base_Texture_Location) uniform texture2D u_baseColorTexture;
    layout(set = 0, binding = Base_Sampler_Location) uniform sampler u_baseColorSampler;
#endif

layout(location = V_UV) in vec2 v_uv;
layout(location = 0) out vec4 o_color;

void main() {
    vec4 baseColor = base_color;
    
#ifdef HAS_BASE_TEXTURE
    vec4 textureColor = texture(sampler2D(u_baseColorTexture, u_baseColorSampler), v_uv);
    baseColor *= textureColor;
#endif
    
#ifdef NEED_ALPHA_CUTOFF
    if (base_color.a < alpha_cutoff) {
        discard;
    }
#endif
    
    o_color = baseColor;
}
