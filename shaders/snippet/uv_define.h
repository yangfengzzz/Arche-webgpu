#ifdef Vert_Shader
    layout (location = V_UV) out vec2 v_uv;
#else
    layout (location = V_UV) in vec2 v_uv;
#endif

#ifdef HAS_UV1
    #ifdef Vert_Shader
        layout (location = V_UV1) out vec2 v_uv1;
    #else
        layout (location = V_UV1) in vec2 v_uv1;
    #endif
#endif