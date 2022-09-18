// directional light
#ifdef DIRECT_LIGHT_COUNT
    struct DirectLight {
        vec3 color;
        vec3 direction;
    };

    layout(set = 0, binding = Direct_Light_Location) uniform u_directLight {
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

    layout(set = 0, binding = Point_Light_Location) uniform u_pointLight {
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

    layout(set = 0, binding = Spot_Light_Location) uniform u_spotLight {
        SpotLight spot_light[SPOT_LIGHT_COUNT];
    };
#endif

// ambient light
layout(set = 0, binding = EnvMap_Light_Location) uniform u_envMapLight {
    vec3 diffuse;
    float mip_map_level;
    float diffuse_intensity;
    float specular_intensity;
} env_map_light;

#ifdef HAS_SH
    layout(set = 0, binding = Env_SH_Location) uniform u_env_sh {
        vec3 env_sh[9];
    };
#endif

#ifdef HAS_SPECULAR_ENV
    layout(set = 0, binding = Env_Specular_Texture_Location) uniform textureCube u_env_specularTexture;
    layout(set = 0, binding = Env_Specular_Sampler_Location) uniform sampler u_env_specularSampler;
#endif
