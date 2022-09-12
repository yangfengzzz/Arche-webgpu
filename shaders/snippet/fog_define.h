#ifdef HAS_FOG

varying vec3 v_fogDepth;

uniform vec3 u_fogColor;

    #ifdef FOG_EXP2

        uniform float u_fogDensity;

    #else

        uniform float u_fogNear;
        uniform float u_fogFar;

    #endif

#endif
