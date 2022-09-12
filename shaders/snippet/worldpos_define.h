#ifdef NEED_WORLDPOS
    #ifdef Vert_Shader
        layout (location = V_Pos) out vec3 v_pos;
    #else
        layout (location = V_Pos) in vec3 v_pos;
    #endif
#endif
