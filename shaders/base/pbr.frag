#version 450

#define PI 3.14159265359
#define RECIPROCAL_PI 0.31830988618
#define EPSILON 1e-6
#define LOG2 1.442695

float saturate(float t) {
    return clamp(t, 0.0, 1.0);
}

float whiteCompliment(float t) {
    return 1.0 - saturate(t);
}

vec4 RGBMToLinear(vec4 value, float maxRange) {
    return vec4(value.rgb * value.a * maxRange, 1.0);
}

vec4 gammaToLinear(vec4 srgbIn){
    return vec4(pow(srgbIn.rgb, vec3(2.2)), srgbIn.a);
}

vec4 linearToGamma(vec4 linearIn){
    return vec4(pow(linearIn.rgb, vec3(1.0 / 2.2)), linearIn.a);
}

layout(set = 0, binding = 5) uniform cameraData {
    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 camera_pos;
} camera_data;

layout(set = 0, binding = 6) uniform rendererData {
    mat4 local_mat;
    mat4 model_mat;
    mat4 normal_mat;
} renderer_data;

layout (location = 0) in vec2 v_uv;

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_NORMAL
    #if defined(HAS_TANGENT) && defined(HAS_NORMAL_TEXTURE)
        layout (location = 2) in mat3 v_TBN;
    #else
        layout (location = 3) in vec3 v_normal;
    #endif
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef HAS_VERTEXCOLOR
    layout (location = 1) in vec4 v_color;
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef NEED_WORLDPOS
    layout (location = 4) in vec3 v_pos;
#endif

//----------------------------------------------------------------------------------------------------------------------
// directional light
#ifdef DIRECT_LIGHT_COUNT
    struct DirectLight {
        vec3 color;
        vec3 direction;
    };

    layout(set = 0, binding = 7) uniform directLight {
        DirectLight value[DIRECT_LIGHT_COUNT];
    } direct_light;
#endif

// point light
#ifdef POINT_LIGHT_COUNT
    struct PointLight {
        vec3 color;
        vec3 position;
        float distance;
    };

    layout(set = 0, binding = 8) uniform pointLight {
        PointLight value[POINT_LIGHT_COUNT];
    } point_light;
#endif

// spot light
#ifdef SPOT_LIGHT_COUNT
    struct SpotLight {
        vec3 color;
        vec3 position;
        vec3 direction;
        float distance;
        float angleCos;
        float penumbraCos;
    };

    layout(set = 0, binding = 9) uniform spotLight {
        SpotLight value[SPOT_LIGHT_COUNT];
    } spot_light;
#endif

// ambient light
layout(set = 0, binding = 10) uniform envMapLight {
    vec3 diffuse;
    float mip_map_level;
    float diffuse_intensity;
    float specular_intensity;
} env_map_light;

#ifdef HAS_SH
    layout(set = 0, binding = 11) uniform envSH {
        vec3 value[9];
    } env_sh;
#endif

#ifdef HAS_SPECULAR_ENV
    layout(set = 0, binding = 12) uniform samplerCube env_specularTexture;
#endif

//----------------------------------------------------------------------------------------------------------------------
layout(set = 0, binding = 13) uniform alphaCutoff {
    float value;
} alpha_cutoff;

layout(set = 0, binding = 14) uniform pbrBaseData {
    vec4 base_color;
    vec4 emissive_color;
    float normal_texture_intensity;
    float occlusion_texture_intensity;
} pbr_base_data;

layout(set = 0, binding = 15) uniform pbrData {
    float metal;
    float roughness;
} pbr_data;

layout(set = 0, binding = 16) uniform pbrSpecularData {
    vec3 specular_color;
    float glossiness;
} pbr_specular_data;

#ifdef HAS_BASECOLORMAP
    layout(set = 0, binding = 17) uniform sampler2D baseColorTexture;
#endif

#ifdef HAS_NORMAL_TEXTURE
    layout(set = 0, binding = 18) uniform sampler2D normalTexture;
#endif

#ifdef HAS_EMISSIVEMAP
    layout(set = 0, binding = 19) uniform sampler2D emissiveTexture;
#endif

#ifdef HAS_METALROUGHNESSMAP
    layout(set = 0, binding = 20) uniform sampler2D metallicRoughnessTexture;
#endif

#ifdef HAS_SPECULARGLOSSINESSMAP
    layout(set = 0, binding = 21) uniform sampler2D specularGlossinessTexture;
#endif

#ifdef HAS_OCCLUSIONMAP
    layout(set = 0, binding = 22) uniform sampler2D occlusionTexture;
#endif

struct ReflectedLight {
    vec3 directDiffuse;
    vec3 directSpecular;
    vec3 indirectDiffuse;
    vec3 indirectSpecular;
};
struct GeometricContext {
    vec3  position;
    vec3  normal;
    vec3  viewDir;
};
struct PhysicalMaterial {
    vec3  diffuseColor;
    float roughness;
    vec3  specularColor;
    float opacity;
};

//----------------------------------------------------------------------------------------------------------------------
vec3 getNormal() {
    #ifdef HAS_NORMAL_TEXTURE
        #ifndef HAS_TANGENT
            #ifdef HAS_DERIVATIVES
                vec3 pos_dx = dFdx(v_pos);
                vec3 pos_dy = dFdy(v_pos);
                vec3 tex_dx = dFdx(vec3(v_uv, 0.0));
                vec3 tex_dy = dFdy(vec3(v_uv, 0.0));
                vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);
                #ifdef HAS_NORMAL
                    vec3 ng = normalize(v_normal);
                #else
                    vec3 ng = normalize(cross(pos_dx, pos_dy));
                #endif
                t = normalize(t - ng * dot(ng, t));
                vec3 b = normalize(cross(ng, t));
                mat3 tbn = mat3(t, b, ng);
            #else
                #ifdef HAS_NORMAL
                    vec3 ng = normalize(v_normal);
                #else
                    vec3 ng = vec3(0.0, 0.0, 1.0);
                #endif
                mat3 tbn = mat3(vec3(0.0), vec3(0.0), ng);
            #endif
        #else
            mat3 tbn = v_TBN;
        #endif
        vec3 n = texture(normalTexture, v_uv).rgb;
        n = normalize(tbn * ((2.0 * n - 1.0) * vec3(blinn_phong_data.normal_intensity, blinn_phong_data.normal_intensity, 1.0)));
    #else
        #ifdef HAS_NORMAL
            vec3 n = normalize(v_normal);
        #elif defined(HAS_DERIVATIVES)
            vec3 pos_dx = dFdx(v_pos);
            vec3 pos_dy = dFdy(v_pos);
            vec3 n = normalize(cross(pos_dx, pos_dy));
        #else
            vec3 n= vec3(0.0, 0.0, 1.0);
        #endif
    #endif

    n *= float(gl_FrontFacing) * 2.0 - 1.0;
    return n;
}

//----------------------------------------------------------------------------------------------------------------------
float pow2(float x) {
    return x * x;
}

vec3 BRDF_Diffuse_Lambert(vec3 diffuseColor) {
    return RECIPROCAL_PI * diffuseColor;
}

float computeSpecularOcclusion(float ambientOcclusion, float roughness, float dotNV) {
    return saturate(pow(dotNV + ambientOcclusion, exp2(- 16.0 * roughness - 1.0)) - 1.0 + ambientOcclusion);
}

PhysicalMaterial getPhysicalMaterial(vec4 diffuseColor, float metal, float roughness, 
vec3 specularColor, float glossiness, float alphaCutoff){
    PhysicalMaterial material;

    #ifdef HAS_BASECOLORMAP
        vec4 baseColor = texture(baseColorTexture, v_uv);
//        #ifndef OASIS_COLORSPACE_GAMMA
//            baseColor = gammaToLinear(baseColor);
//        #endif
        diffuseColor *= baseColor;
    #endif

    #ifdef HAS_VERTEXCOLOR
        diffuseColor *= v_color;
    #endif


    #ifdef NEED_ALPHA_CUTOFF
        if (diffuseColor.a < alphaCutoff) {
            discard;
        }
    #endif

    #ifdef HAS_METALROUGHNESSMAP
        vec4 metalRoughMapColor = texture(metallicRoughnessTexture, v_uv);
        roughness *= metalRoughMapColor.g;
        metal *= metalRoughMapColor.b;
    #endif

    #ifdef HAS_SPECULARGLOSSINESSMAP
        vec4 specularGlossinessColor = texture(specularGlossinessTexture, v_uv);
        specularColor *= specularGlossinessColor.rgb;
        glossiness *= specularGlossinessColor.a;
    #endif


    #ifdef IS_METALLIC_WORKFLOW
        material.diffuseColor = diffuseColor.rgb * (1.0 - metal);
        material.specularColor = mix(vec3(0.04), diffuseColor.rgb, metal);
        material.roughness = clamp(roughness, 0.04, 1.0);
    #else
        float specularStrength = max(max(specularColor.r, specularColor.g), specularColor.b);
        material.diffuseColor = diffuseColor.rgb * (1.0 - specularStrength);
        material.specularColor = specularColor;
        material.roughness = clamp(1.0 - glossiness, 0.04, 1.0);
    #endif

    material.opacity = diffuseColor.a;
    return material;
}

//----------------------------------------------------------------------------------------------------------------------
vec3 F_Schlick(vec3 specularColor, float dotLH) {
    // Original approximation by Christophe Schlick '94
    // float fresnel = pow( 1.0 - dotLH, 5.0 );

    // Optimized variant (presented by Epic at SIGGRAPH '13)
    // https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
    float fresnel = exp2((-5.55473 * dotLH - 6.98316) * dotLH);

    return (1.0 - specularColor) * fresnel + specularColor;
}

// Moving Frostbite to Physically Based Rendering 3.0 - page 12, listing 2
// https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf
float G_GGX_SmithCorrelated(float alpha, float dotNL, float dotNV) {
    float a2 = pow2(alpha);

    // dotNL and dotNV are explicitly swapped. This is not a mistake.
    float gv = dotNL * sqrt(a2 + (1.0 - a2) * pow2(dotNV));
    float gl = dotNV * sqrt(a2 + (1.0 - a2) * pow2(dotNL));

    return 0.5 / max(gv + gl, EPSILON);
}

// Microfacet Models for Refraction through Rough Surfaces - equation (33)
// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// alpha is "roughness squared" in Disney’s reparameterization
float D_GGX(float alpha, float dotNH) {
    float a2 = pow2(alpha);

    float denom = pow2(dotNH) * (a2 - 1.0) + 1.0;// avoid alpha = 0 with dotNH = 1

    return RECIPROCAL_PI * a2 / pow2(denom);
}

// GGX Distribution, Schlick Fresnel, GGX-Smith Visibility
vec3 BRDF_Specular_GGX(vec3 incidentDirection, GeometricContext geometry, vec3 specularColor, float roughness) {
    float alpha = pow2(roughness);// UE4's roughness

    vec3 halfDir = normalize(incidentDirection + geometry.viewDir);

    float dotNL = saturate(dot(geometry.normal, incidentDirection));
    float dotNV = saturate(dot(geometry.normal, geometry.viewDir));
    float dotNH = saturate(dot(geometry.normal, halfDir));
    float dotLH = saturate(dot(incidentDirection, halfDir));

    vec3 F = F_Schlick(specularColor, dotLH);

    float G = G_GGX_SmithCorrelated(alpha, dotNL, dotNV);

    float D = D_GGX(alpha, dotNH);

    return F * (G * D);
}

//----------------------------------------------------------------------------------------------------------------------
void addDirectRadiance(vec3 incidentDirection, vec3 color, GeometricContext geometry, PhysicalMaterial material, inout ReflectedLight reflectedLight) {
    float dotNL = saturate(dot(geometry.normal, incidentDirection));

    vec3 irradiance = dotNL * color;
    irradiance *= PI;

    reflectedLight.directSpecular += irradiance * BRDF_Specular_GGX(incidentDirection, geometry, material.specularColor, material.roughness);

    reflectedLight.directDiffuse += irradiance * BRDF_Diffuse_Lambert(material.diffuseColor);
}

#ifdef DIRECT_LIGHT_COUNT
    void addDirectionalDirectLightRadiance(DirectLight directionalLight, GeometricContext geometry, PhysicalMaterial material, inout ReflectedLight reflectedLight) {
        vec3 color = directionalLight.color;
        vec3 direction = -directionalLight.direction;

        addDirectRadiance(direction, color, geometry, material, reflectedLight);
    }
#endif

#ifdef POINT_LIGHT_COUNT
    void addPointDirectLightRadiance(PointLight pointLight, GeometricContext geometry, PhysicalMaterial material, inout ReflectedLight reflectedLight) {
        vec3 lVector = pointLight.position - geometry.position;
        vec3 direction = normalize(lVector);

        float lightDistance = length(lVector);

        vec3 color = pointLight.color;
        color *= clamp(1.0 - pow(lightDistance/pointLight.distance, 4.0), 0.0, 1.0);

        addDirectRadiance(direction, color, geometry, material, reflectedLight);
    }
 #endif

#ifdef SPOT_LIGHT_COUNT
    void addSpotDirectLightRadiance(SpotLight spotLight, GeometricContext geometry, PhysicalMaterial material, inout ReflectedLight reflectedLight) {
        vec3 lVector = spotLight.position - geometry.position;
        vec3 direction = normalize(lVector);

        float lightDistance = length(lVector);
        float angleCos = dot(direction, -spotLight.direction);

        float spotEffect = smoothstep(spotLight.penumbraCos, spotLight.angleCos, angleCos);
        float decayEffect = clamp(1.0 - pow(lightDistance/spotLight.distance, 4.0), 0.0, 1.0);

        vec3 color = spotLight.color;
        color *= spotEffect * decayEffect;

        addDirectRadiance(direction, color, geometry, material, reflectedLight);
    }
#endif

void addTotalDirectRadiance(GeometricContext geometry, PhysicalMaterial material, inout ReflectedLight reflectedLight){
    #ifdef DIRECT_LIGHT_COUNT
        DirectLight directionalLight;
        for (int i = 0; i < DIRECT_LIGHT_COUNT; i ++) {
            directionalLight = direct_light.value[i];

            addDirectionalDirectLightRadiance(directionalLight, geometry, material, reflectedLight);
        }
    #endif

    #ifdef POINT_LIGHT_COUNT
        PointLight pointLight;
        for (int i = 0; i < POINT_LIGHT_COUNT; i ++) {
            pointLight = point_light.value[i];

            addPointDirectLightRadiance(pointLight, geometry, material, reflectedLight);
        }
    #endif

    #ifdef SPOT_LIGHT_COUNT
        SpotLight spotLight;
        for (int i = 0; i < SPOT_LIGHT_COUNT; i ++) {
            spotLight = spot_light.value[i];

            addSpotDirectLightRadiance(spotLight, geometry, material, reflectedLight);
        }
    #endif
}

//----------------------------------------------------------------------------------------------------------------------
// ------------------------Diffuse------------------------
// sh need be pre-scaled in CPU.
vec3 getLightProbeIrradiance(vec3 sh[9], vec3 normal){
    vec3 result = sh[0] +

    sh[1] * (normal.y) +
    sh[2] * (normal.z) +
    sh[3] * (normal.x) +

    sh[4] * (normal.y * normal.x) +
    sh[5] * (normal.y * normal.z) +
    sh[6] * (3.0 * normal.z * normal.z - 1.0) +
    sh[7] * (normal.z * normal.x) +
    sh[8] * (normal.x * normal.x - normal.y * normal.y);

    return max(result, vec3(0.0));
}

// ------------------------Specular------------------------
// ref: https://www.unrealengine.com/blog/physically-based-shading-on-mobile - environmentBRDF for GGX on mobile
vec3 envBRDFApprox(vec3 specularColor, float roughness, float dotNV) {
    const vec4 c0 = vec4(- 1, - 0.0275, - 0.572, 0.022);

    const vec4 c1 = vec4(1, 0.0425, 1.04, - 0.04);

    vec4 r = roughness * c0 + c1;

    float a004 = min(r.x * r.x, exp2(- 9.28 * dotNV)) * r.x + r.y;

    vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;

    return specularColor * AB.x + AB.y;
}


float getSpecularMIPLevel(float roughness, int maxMIPLevel) {
    return roughness * float(maxMIPLevel);
}

vec3 getLightProbeRadiance(GeometricContext geometry, float roughness, int maxMIPLevel, float specularIntensity) {
    #ifndef HAS_SPECULAR_ENV
        return vec3(0);
    #else
        vec3 reflectVec = reflect(-geometry.viewDir, geometry.normal);
        float specularMIPLevel = getSpecularMIPLevel(roughness, maxMIPLevel);

        #ifdef HAS_TEX_LOD
            vec4 envMapColor = textureCubeLodEXT(env_specularTexture, reflectVec, specularMIPLevel);
        #else
            vec4 envMapColor = texture(env_specularTexture, reflectVec, specularMIPLevel);
        #endif

        #ifdef DECODE_ENV_RGBM
            envMapColor.rgb = RGBMToLinear(envMapColor, 5.0).rgb;
//            #ifdef OASIS_COLORSPACE_GAMMA
//                envMapColor = linearToGamma(envMapColor);
//            #endif
        #else
//            #ifndef OASIS_COLORSPACE_GAMMA
//                envMapColor = gammaToLinear(envMapColor);
//            #endif
        #endif

        return envMapColor.rgb * specularIntensity;
    #endif
}

layout(location = 0) out vec4 o_color;
//----------------------------------------------------------------------------------------------------------------------
//**********************************************************************************************************************
//----------------------------------------------------------------------------------------------------------------------
void main() {
    GeometricContext geometry = GeometricContext(v_pos, getNormal(), normalize(camera_data.camera_pos - v_pos));
    PhysicalMaterial material = getPhysicalMaterial(pbr_base_data.base_color, pbr_data.metal, pbr_data.roughness,
    pbr_specular_data.specular_color, pbr_specular_data.glossiness, alpha_cutoff.value);
    ReflectedLight reflectedLight = ReflectedLight(vec3(0), vec3(0), vec3(0), vec3(0));
    float dotNV = saturate(dot(geometry.normal, geometry.viewDir));

    // Direct Light
    addTotalDirectRadiance(geometry, material, reflectedLight);

    // IBL diffuse
    #ifdef HAS_SH
        vec3 irradiance = getLightProbeIrradiance(env_sh.value, geometry.normal);
//        #ifdef OASIS_COLORSPACE_GAMMA
//            irradiance = linearToGamma(vec4(irradiance, 1.0)).rgb;
//        #endif
        irradiance *= env_map_light.diffuse_intensity;
    #else
        vec3 irradiance = env_map_light.diffuse * env_map_light.diffuse_intensity;
        irradiance *= PI;
    #endif

    reflectedLight.indirectDiffuse += irradiance * BRDF_Diffuse_Lambert(material.diffuseColor);

    // IBL specular
    vec3 radiance = getLightProbeRadiance(geometry, material.roughness, int(env_map_light.mip_map_level), env_map_light.specular_intensity);
    reflectedLight.indirectSpecular += radiance * envBRDFApprox(material.specularColor, material.roughness, dotNV);

    // Occlusion
    #ifdef HAS_OCCLUSIONMAP
        vec2 aoUV = v_uv;
        float ambientOcclusion = (texture(occlusionTexture, aoUV).r - 1.0) + 1.0;
        reflectedLight.indirectDiffuse *= ambientOcclusion;
        #ifdef HAS_SPECULAR_ENV
            reflectedLight.indirectSpecular *= computeSpecularOcclusion(ambientOcclusion, material.roughness, dotNV);
        #endif
    #endif


    // Emissive
    vec3 emissiveRadiance = pbr_base_data.emissive_color.rgb;
    #ifdef HAS_EMISSIVEMAP
        vec4 emissiveColor = texture(emissiveTexture, v_uv);
//        #ifndef OASIS_COLORSPACE_GAMMA
//            emissiveColor = gammaToLinear(emissiveColor);
//        #endif
        emissiveRadiance *= emissiveColor.rgb;
    #endif

    // Total
    vec3 totalRadiance = reflectedLight.directDiffuse +
    reflectedLight.indirectDiffuse +
    reflectedLight.directSpecular +
    reflectedLight.indirectSpecular +
    emissiveRadiance;

    vec4 targetColor = vec4(totalRadiance, material.opacity);
//    #ifndef OASIS_COLORSPACE_GAMMA
//        targetColor = linearToGamma(targetColor);
//    #endif
    o_color = targetColor;
}