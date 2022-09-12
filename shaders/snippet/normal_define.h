#ifndef OMIT_NORMAL
    #ifdef HAS_NORMAL
        layout (location = 2) out vec3 v_normal;
        #if defined(HAS_TANGENT) && ( defined(NORMALTEXTURE) || defined(HAS_CLEARCOATNORMALTEXTURE) )
            layout (location = 3) out mat3 v_TBN;
        #endif
    #endif
#endif