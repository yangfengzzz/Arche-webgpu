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

layout(binding = 21) buffer u_clusterLights {
    ClusterLightGroup cluster_lights;
};

layout(set = 0, binding = 22) uniform u_clusterUniform {
    vec4 cluster_uniform;
};

//----------------------------------------------------------------------------------------------------------------------
layout(set = 0, binding = 5) uniform u_cameraData {
    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    mat4 view_inv_mat;
    mat4 proj_inv_mat;
    vec3 camera_pos;
};

layout(set = 0, binding = 6) uniform u_rendererData {
    mat4 local_mat;
    mat4 model_mat;
    mat4 normal_mat;
};

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

    layout(set = 0, binding = 9) uniform u_directLight {
        DirectLight direct_light[DIRECT_LIGHT_COUNT];
    };
#endif

// point light
#ifdef POINT_LIGHT_COUNT
    struct PointLight {
        vec3 color;
        vec3 position;
        float distance;
    };

    layout(set = 0, binding = 10) uniform u_pointLight {
        PointLight point_light[POINT_LIGHT_COUNT];
    };
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

    layout(set = 0, binding = 11) uniform u_spotLight {
        SpotLight spot_light[SPOT_LIGHT_COUNT];
    };
#endif

// ambient light
layout(set = 0, binding = 12) uniform u_envMapLight {
    vec3 diffuse;
    float mip_map_level;
    float diffuse_intensity;
    float specular_intensity;
};

#ifdef USE_SH
    layout(set = 0, binding = 13) uniform u_env_sH {
        float env_sh[9];
    };
#endif

#ifdef HAS_SPECULAR_ENV
    layout(set = 0, binding = 14) uniform samplerCube u_env_specularTexture;
#endif

//----------------------------------------------------------------------------------------------------------------------
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
    layout(set = 0, binding = 17) uniform sampler2D u_emissiveTexture;
#endif

#ifdef HAS_DIFFUSE_TEXTURE
    layout(set = 0, binding = 18) uniform sampler2D u_diffuseTexture;
#endif

#ifdef HAS_SPECULAR_TEXTURE
    layout(set = 0, binding = 19) uniform sampler2D u_specularTexture;
#endif

#ifdef HAS_NORMAL_TEXTURE
    layout(set = 0, binding = 20) uniform sampler2D u_normalTexture;
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
        n = normalize(tbn * ((2.0 * n - 1.0) * vec3(normal_intensity, normal_intensity, 1.0)));
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
    vec4 emission = emissive_color;
    vec4 diffuse = diffuse_color;
    vec4 specular = specular_color;

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

    ambient = vec4(diffuse * diffuse_intensity, 1.0) * diffuse;

    //------------------------------------------------------------------------------------------------------------------
    #ifdef NEED_WORLDPOS
        vec3 V =  normalize(camera_pos - v_pos);
    #endif

    //------------------------------------------------------------------------------------------------------------------
    vec3 N = getNormal();
    vec3 lightDiffuse = vec3(0.0, 0.0, 0.0);
    vec3 lightSpecular = vec3(0.0, 0.0, 0.0);

    #ifdef DIRECT_LIGHT_COUNT
        DirectLight directionalLight;
        for (int i = 0; i < DIRECT_LIGHT_COUNT; i++) {
            directionalLight = direct_light[i];

            float d = max(dot(N, -directionalLight.direction), 0.0);
            lightDiffuse += directionalLight.color * d;

            vec3 halfDir = normalize(V - directionalLight.direction);
            float s = pow(clamp(dot(N, halfDir), 0.0, 1.0), shininess);
            lightSpecular += directionalLight.color * s;
        }
    #endif

    uint clusterIndex = 0;
    uint lightOffset = 0;
    #ifdef NEED_FORWARD_PLUS
        clusterIndex = getClusterIndex(cluster_uniform, gl_FragCoord);
        lightOffset  = cluster_lights.lights[clusterIndex].offset;
    #endif

    #ifdef POINT_LIGHT_COUNT
        uint lightCount = POINT_LIGHT_COUNT;
        #ifdef NEED_FORWARD_PLUS
            lightCount = cluster_lights.lights[clusterIndex].point_count;
        #endif

        PointLight pointLight;
        for (int i = 0; i < lightCount; i++) {
            uint index = i;
            #ifdef NEED_FORWARD_PLUS
                index = cluster_lights.indices[lightOffset + i];
            #endif
            pointLight = point_light[index];

            vec3 direction = v_pos - pointLight.position;
            float dist = length(direction);
            direction /= dist;
            float decay = clamp(1.0 - pow(dist / pointLight.distance, 4.0), 0.0, 1.0);

            float d =  max(dot(N, -direction), 0.0) * decay;
            lightDiffuse += pointLight.color * d;

            vec3 halfDir = normalize(V - direction);
            float s = pow(clamp(dot(N, halfDir), 0.0, 1.0), shininess)  * decay;
            lightSpecular += pointLight.color * s;
        }
    #endif

    #ifdef SPOT_LIGHT_COUNT
        uint pointlightCount;
        uint lightCount2 = SPOT_LIGHT_COUNT;
        #ifdef NEED_FORWARD_PLUS
            pointlightCount = cluster_lights.lights[clusterIndex].point_count;
            lightCount2 = cluster_lights.lights[clusterIndex].spot_count;
        #endif

        SpotLight spotLight;
        for (int i = 0; i < lightCount2; i++) {
            uint index = i;
            #ifdef NEED_FORWARD_PLUS
                index = cluster_lights.indices[lightOffset + i + pointlightCount];
            #endif
            spotLight = spot_light[index];

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
            float s = pow(clamp(dot(N, halfDir), 0.0, 1.0), shininess) * decayTotal;
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
        if (diffuse.a < alpha_cutoff) {
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