layout(set = 0, binding = Blinn_Phong_Data_Location) uniform u_blinnPhongData {
    vec4 u_baseColor;
    vec4 u_specularColor;
    vec4 u_emissiveColor;
    float u_normalIntensity;
    float u_shininess;
};

layout(set = 0, binding = Alpha_Cutoff_Location) uniform u_alphaCutoff {
    float alpha_cutoff;
};

#ifdef HAS_EMISSIVE_TEXTURE
layout(set = 0, binding = Emissive_Texture_Location) uniform texture2D u_emissiveTexture;
layout(set = 0, binding = Emissive_Sampler_Location) uniform sampler u_emissiveSampler;
#endif

#ifdef HAS_BASE_TEXTURE
layout(set = 0, binding = Base_Texture_Location) uniform texture2D u_baseColorTexture;
layout(set = 0, binding = Base_Sampler_Location) uniform sampler u_baseColorSampler;
#endif

#ifdef HAS_SPECULAR_TEXTURE
layout(set = 0, binding = Specular_Texture_Location) uniform texture2D u_specularTexture;
layout(set = 0, binding = Specular_Sampler_Location) uniform sampler u_specularSampler;
#endif

#ifdef HAS_NORMAL_TEXTURE
layout(set = 0, binding = Normal_Texture_Location) uniform texture2D u_normalTexture;
layout(set = 0, binding = Normal_Sampler_Location) uniform sampler u_normalSampler;
#endif
