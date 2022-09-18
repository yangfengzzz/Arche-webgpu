#ifndef OMIT_NORMAL
    #ifdef HAS_NORMAL
        #ifdef Vert_Shader
            layout (location = V_Normal) out vec3 v_normal;
        #else
            layout (location = V_Normal) in vec3 v_normal;
        #endif
        #if defined(HAS_TANGENT) && ( defined(HAS_NORMAL_TEXTURE) || defined(HAS_CLEARCOATNORMAL_TEXTURE) )
            #ifdef Vert_Shader
                layout (location = V_TBN) out mat3 v_TBN;
            #else
                layout (location = V_TBN) in mat3 v_TBN;
            #endif
        #endif
    #endif
#endif
