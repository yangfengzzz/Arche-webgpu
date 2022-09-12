#ifdef HAS_VERTEXCOLOR
    #ifdef Vert_Shader
        layout (location = V_Color) out vec4 v_color;
    #else
        layout (location = V_Color) in vec4 v_color;
    #endif
#endif
