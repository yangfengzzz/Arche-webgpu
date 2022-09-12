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
} env_map_light;

#ifdef USE_SH
    layout(set = 0, binding = 13) uniform u_env_sH {
        float env_sh[9];
    };
#endif

#ifdef HAS_SPECULAR_ENV
    layout(set = 0, binding = 14) uniform textureCube u_env_specularTexture;
    layout(set = 0, binding = 15) uniform sampler u_env_specularSampler;
#endif
