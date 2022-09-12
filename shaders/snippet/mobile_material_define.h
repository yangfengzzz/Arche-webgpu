layout(set = 0, binding = 15) uniform u_blinnPhongData {
    vec4 diffuse_color;
    vec4 specular_color;
    vec4 emissive_color;
    float normal_intensity;
    float shininess;
};

layout(set = 0, binding = 16) uniform u_alphaCutoff {
    float alpha_cutoff;
};

#ifdef HAS_EMISSIVE_TEXTURE
layout(set = 0, binding = 17) uniform texture2D u_emissiveTexture;
layout(set = 0, binding = 18) uniform sampler u_emissiveSampler;
#endif

#ifdef HAS_DIFFUSE_TEXTURE
layout(set = 0, binding = 18) uniform texture2D u_diffuseTexture;
layout(set = 0, binding = 18) uniform sampler u_diffuseSampler;
#endif

#ifdef HAS_SPECULAR_TEXTURE
layout(set = 0, binding = 19) uniform texture2D u_specularTexture;
layout(set = 0, binding = 19) uniform sampler u_specularSampler;
#endif

#ifdef HAS_NORMAL_TEXTURE
layout(set = 0, binding = 20) uniform texture2D u_normalTexture;
layout(set = 0, binding = 20) uniform sampler u_normalSampler;
#endif
