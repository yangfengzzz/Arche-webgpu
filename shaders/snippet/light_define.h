// directional light
#ifdef DIRECT_LIGHT_COUNT

    struct DirectLight {
        vec3 color;
        vec3 direction;
    };

    uniform vec3 u_directLightColor[DIRECT_LIGHT_COUNT];
    uniform vec3 u_directLightDirection[DIRECT_LIGHT_COUNT];

#endif


// point light
#ifdef POINT_LIGHT_COUNT

    struct PointLight {
        vec3 color;
        vec3 position;
        float distance;
    };

    uniform vec3 u_pointLightColor[ POINT_LIGHT_COUNT ];
    uniform vec3 u_pointLightPosition[ POINT_LIGHT_COUNT ];
    uniform float u_pointLightDistance[ POINT_LIGHT_COUNT ];

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

    uniform vec3 u_spotLightColor[ SPOT_LIGHT_COUNT ];
    uniform vec3 u_spotLightPosition[ SPOT_LIGHT_COUNT ];
    uniform vec3 u_spotLightDirection[ SPOT_LIGHT_COUNT ];
    uniform float u_spotLightDistance[ SPOT_LIGHT_COUNT ];
    uniform float u_spotLightAngleCos[ SPOT_LIGHT_COUNT ];
    uniform float u_spotLightPenumbraCos[ SPOT_LIGHT_COUNT ];

#endif

// ambient light
struct EnvMapLight {
    vec3 diffuse;
    float mipMapLevel;
    float diffuseIntensity;
    float specularIntensity;
};


uniform EnvMapLight u_envMapLight;

#ifdef USE_SH
    uniform vec3 u_env_sh[9];
#endif

#ifdef USE_SPECULAR_ENV
    uniform samplerCube u_env_specularSampler;
#endif
