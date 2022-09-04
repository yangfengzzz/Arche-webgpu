#version 450

#include "base/shadow/shadow.frag"
#include "base/light/cluster_common.comp"

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

layout(binding = 21) buffer clusterLights {
    ClusterLightGroup value;
} cluster_lights;

layout(set = 0, binding = 22) uniform clusterUniform {
    vec4 value;
} cluster_uniform;

//----------------------------------------------------------------------------------------------------------------------
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

#ifdef SHADOW_MAP_COUNT
    layout (location = 5) in vec3 view_pos;
#endif

//----------------------------------------------------------------------------------------------------------------------
// directional light
#ifdef DIRECT_LIGHT_COUNT
    struct DirectLight {
        vec3 color;
        vec3 direction;
    };

    layout(set = 0, binding = 9) uniform directLight {
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

    layout(set = 0, binding = 10) uniform pointLight {
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

    layout(set = 0, binding = 11) uniform spotLight {
        SpotLight value[SPOT_LIGHT_COUNT];
    } spot_light;
#endif

// ambient light
layout(set = 0, binding = 12) uniform envMapLight {
    vec3 diffuse;
    float mip_map_level;
    float diffuse_intensity;
    float specular_intensity;
} env_map_light;

#ifdef USE_SH
    layout(set = 0, binding = 13) uniform env_sh {
        float value[9];
    };
#endif

#ifdef HAS_SPECULAR_ENV
    layout(set = 0, binding = 14) uniform samplerCube env_specularTexture;
#endif

//----------------------------------------------------------------------------------------------------------------------
layout(set = 0, binding = 15) uniform blinnPhongData {
    vec4 diffuse_color;
    vec4 specular_color;
    vec4 emissive_color;
    float normal_intensity;
    float shininess;
} blinn_phong_data;

layout(set = 0, binding = 16) uniform alphaCutoff {
    float value;
} alpha_cutoff;

#ifdef HAS_EMISSIVE_TEXTURE
    layout(set = 0, binding = 17) uniform sampler2D emissiveTexture;
#endif

#ifdef HAS_DIFFUSE_TEXTURE
    layout(set = 0, binding = 18) uniform sampler2D diffuseTexture;
#endif

#ifdef HAS_SPECULAR_TEXTURE
    layout(set = 0, binding = 19) uniform sampler2D specularTexture;
#endif

#ifdef HAS_NORMAL_TEXTURE
    layout(set = 0, binding = 20) uniform sampler2D normalTexture;
#endif

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

//    n *= float(gl_FrontFacing) * 2.0 - 1.0;
    return n;
}

layout(location = 0) out vec4 o_color;

//----------------------------------------------------------------------------------------------------------------------
//**********************************************************************************************************************
//----------------------------------------------------------------------------------------------------------------------
void main() {
    vec4 ambient = vec4(0.0);
    vec4 emission = blinn_phong_data.emissive_color;
    vec4 diffuse = blinn_phong_data.diffuse_color;
    vec4 specular = blinn_phong_data.specular_color;

    #ifdef HAS_EMISSIVE_TEXTURE
        vec4 emissiveTextureColor = texture(emissiveTexture, v_uv);
//        #ifndef OASIS_COLORSPACE_GAMMA
//            emissiveTextureColor = gammaToLinear(emissiveTextureColor);
//        #endif
        emission *= emissiveTextureColor;
    #endif

    #ifdef HAS_DIFFUSE_TEXTURE
        vec4 diffuseTextureColor = texture(diffuseTexture, v_uv);
//        #ifndef OASIS_COLORSPACE_GAMMA
//            diffuseTextureColor = gammaToLinear(diffuseTextureColor);
//        #endif
        diffuse *= diffuseTextureColor;
    #endif

    #ifdef HAS_VERTEXCOLOR
        diffuse *= v_color;
    #endif

    #ifdef HAS_SPECULAR_TEXTURE
        vec4 specularTextureColor = texture(specularTexture, v_uv);
//        #ifndef OASIS_COLORSPACE_GAMMA
//            specularTextureColor = gammaToLinear(specularTextureColor);
//        #endif
        specular *= specularTextureColor;
    #endif

    ambient = vec4(env_map_light.diffuse * env_map_light.diffuse_intensity, 1.0) * diffuse;

    //------------------------------------------------------------------------------------------------------------------
    #ifdef NEED_WORLDPOS
        vec3 V =  normalize(camera_data.camera_pos - v_pos);
    #endif

    //------------------------------------------------------------------------------------------------------------------
    vec3 N = getNormal();
    vec3 lightDiffuse = vec3(0.0, 0.0, 0.0);
    vec3 lightSpecular = vec3(0.0, 0.0, 0.0);

    #ifdef DIRECT_LIGHT_COUNT
        DirectLight directionalLight;
        for (int i = 0; i < DIRECT_LIGHT_COUNT; i++) {
            directionalLight = direct_light.value[i];

            float d = max(dot(N, -directionalLight.direction), 0.0);
            lightDiffuse += directionalLight.color * d;

            vec3 halfDir = normalize(V - directionalLight.direction);
            float s = pow(clamp(dot(N, halfDir), 0.0, 1.0), blinn_phong_data.shininess);
            lightSpecular += directionalLight.color * s;
        }
    #endif

    uint clusterIndex = 0;
    uint lightOffset = 0;
    #ifdef NEED_FORWARD_PLUS
        clusterIndex = getClusterIndex(cluster_uniform.value, gl_FragCoord);
        lightOffset  = cluster_lights.value.lights[clusterIndex].offset;
    #endif

    #ifdef POINT_LIGHT_COUNT
        uint lightCount = POINT_LIGHT_COUNT;
        #ifdef NEED_FORWARD_PLUS
            lightCount = cluster_lights.value.lights[clusterIndex].point_count;
        #endif

        PointLight pointLight;
        for (int i = 0; i < lightCount; i++) {
            uint index = i;
            #ifdef NEED_FORWARD_PLUS
                index = cluster_lights.value.indices[lightOffset + i];
            #endif
            pointLight = point_light.value[index];

            vec3 direction = v_pos - pointLight.position;
            float dist = length(direction);
            direction /= dist;
            float decay = clamp(1.0 - pow(dist / pointLight.distance, 4.0), 0.0, 1.0);

            float d =  max(dot(N, -direction), 0.0) * decay;
            lightDiffuse += pointLight.color * d;

            vec3 halfDir = normalize(V - direction);
            float s = pow(clamp(dot(N, halfDir), 0.0, 1.0), blinn_phong_data.shininess)  * decay;
            lightSpecular += pointLight.color * s;
        }
    #endif

    #ifdef SPOT_LIGHT_COUNT
        uint pointlightCount;
        uint lightCount2 = SPOT_LIGHT_COUNT;
        #ifdef NEED_FORWARD_PLUS
            pointlightCount = cluster_lights.value.lights[clusterIndex].point_count;
            lightCount2 = cluster_lights.value.lights[clusterIndex].spot_count;
        #endif

        SpotLight spotLight;
        for (int i = 0; i < lightCount2; i++) {
            uint index = i;
            #ifdef NEED_FORWARD_PLUS
                index = cluster_lights.value.indices[lightOffset + i + pointlightCount];
            #endif
            spotLight = spot_light.value[index];

            vec3 direction = spotLight.position - v_pos;
            float lightDistance = length(direction);
            direction /= lightDistance;
            float angleCos = dot(direction, -spotLight.direction);
            float decay = clamp(1.0 - pow(lightDistance/spotLight.distance, 4.0), 0.0, 1.0);
            float spotEffect = smoothstep(spotLight.penumbraCos, spotLight.angleCos, angleCos);
            float decayTotal = decay * spotEffect;
            float d = max(dot(N, direction), 0.0)  * decayTotal;
            lightDiffuse += spotLight.color * d;

            vec3 halfDir = normalize(V + direction);
            float s = pow(clamp(dot(N, halfDir), 0.0, 1.0), blinn_phong_data.shininess) * decayTotal;
            lightSpecular += spotLight.color * s;
        }
    #endif

    diffuse *= vec4(lightDiffuse, 1.0);
    specular *= vec4(lightSpecular, 1.0);

    #ifdef SHADOW_MAP_COUNT
        float shadow = 0;
        for( int i = 0; i < SHADOW_MAP_COUNT; i++) {
            shadow += filterPCF(v_pos, view_pos, i);
            // shadow += textureProj(v_pos, view_pos, vec2(0), i);
        }
        shadow /= SHADOW_MAP_COUNT;
        diffuse *= shadow;
        specular *= shadow;
    #endif

    #ifdef NEED_ALPHA_CUTOFF
        if (diffuse.a < alpha_cutoff.value) {
            discard;
        }
    #endif

    //------------------------------------------------------------------------------------------------------------------
    o_color = emission + ambient + diffuse + specular;
    o_color.a = diffuse.a;

//    #ifndef OASIS_COLORSPACE_GAMMA
//        gl_FragColor = linearToGamma(gl_FragColor);
//    #endif
}