layout(set = 0, binding = Alpha_Cutoff_Location) uniform u_alphaCutoff {
    float alpha_cutoff;
};

layout(set = 0, binding = PBR_Base_Data_Location) uniform u_pbrBaseData {
    vec4 u_baseColor;
    vec4 u_emissiveColor;
    float u_normalIntensity;
    float u_occlusionIntensity;
    float u_clearCoat;
    float u_clearCoatRoughness;
};

layout(set = 0, binding = PBR_Data_Location) uniform u_pbrData {
    float u_metal;
    float u_roughness;
};

layout(set = 0, binding = PBR_Specular_Data_Data_Location) uniform u_pbrSpecularData {
    vec3 u_PBRSpecularColor;
    float u_glossiness;
};

// Texture
#ifdef BASETEXTURE
    layout(set = 0, binding = Base_Texture_Location) uniform texture2D u_baseTexture;
    layout(set = 0, binding = Base_Sampler_Location) uniform sampler u_baseSampler;
#endif

#ifdef NORMALTEXTURE
    layout(set = 0, binding = Normal_Texture_Location) uniform texture2D u_normalTexture;
    layout(set = 0, binding = Normal_Sampler_Location) uniform sampler u_normalSampler;
#endif

#ifdef EMISSIVETEXTURE
    layout(set = 0, binding = Emissive_Texture_Location) uniform texture2D u_emissiveTexture;
    layout(set = 0, binding = Emissive_Sampler_Location) uniform sampler u_emissiveSampler;
#endif

#ifdef ROUGHNESSMETALLICTEXTURE
    layout(set = 0, binding = Roughness_Metallic_Texture_Location) uniform texture2D u_roughnessMetallicTexture;
    layout(set = 0, binding = Roughness_Metallic_Sampler_Location) uniform sampler u_roughnessMetallicSampler;
#endif

#ifdef SPECULARGLOSSINESSTEXTURE
    layout(set = 0, binding = Specular_Glossiness_Texture_Location) uniform texture2D u_specularGlossinessTexture;
    layout(set = 0, binding = Specular_Glossiness_Sampler_Location) uniform sampler u_specularGlossinessSampler;
#endif

#ifdef OCCLUSIONTEXTURE
    layout(set = 0, binding = Occlusion_Texture_Location) uniform texture2D u_occlusionTexture;
    layout(set = 0, binding = Occlusion_Sampler_Location) uniform sampler u_occlusionSampler;
#endif

#ifdef HAS_CLEARCOATTEXTURE
    layout(set = 0, binding = Clear_Coat_Texture_Location) uniform texture2D u_clearCoatTexture;
    layout(set = 0, binding = Clear_Coat_Sampler_Location) uniform sampler u_clearCoatSampler;
#endif

#ifdef HAS_CLEARCOATROUGHNESSTEXTURE
    layout(set = 0, binding = Clear_Coat_Roughness_Texture_Location) uniform texture2D u_clearCoatRoughnessTexture;
    layout(set = 0, binding = Clear_Coat_Roughness_Sampler_Location) uniform sampler u_clearCoatRoughnessSampler;
#endif

#ifdef HAS_CLEARCOATNORMALTEXTURE
    layout(set = 0, binding = Clear_Coat_Normal_Texture_Location) uniform texture2D u_clearCoatNormalTexture;
    layout(set = 0, binding = Clear_Coat_Normal_Sampler_Location) uniform sampler u_clearCoatNormalSampler;
#endif

// Runtime
struct ReflectedLight {
    vec3 directDiffuse;
    vec3 directSpecular;
    vec3 indirectDiffuse;
    vec3 indirectSpecular;
};

struct Geometry {
    vec3  position;
    vec3  normal;
    vec3  viewDir;
    float dotNV;
    
    #ifdef CLEARCOAT
        vec3 clearCoatNormal;
        float clearCoatDotNV;
    #endif

};

struct Material {
    vec3  diffuseColor;
    float roughness;
    vec3  specularColor;
    float opacity;
    #ifdef CLEARCOAT
        float clearCoat;
        float clearCoatRoughness;
    #endif
};
