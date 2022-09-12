#ifdef HAS_FOG
    #ifdef Vert_Shader
        layout (location = V_Fog_Depth) out vec3 v_fogDepth;
    #else
        layout (location = V_Fog_Depth) in vec3 v_fogDepth;
    #endif

    layout(set = 0, binding = Fog_Color_Location) vec3 u_fogColor;
    #ifdef FOG_EXP2
        layout(set = 0, binding = Fog_Density_Location) float u_fogDensity;
    #else
        layout(set = 0, binding = Fog_Near_Location) float u_fogNear;
        layout(set = 0, binding = Fog_Far_Location) float u_fogFar;
    #endif
#endif
