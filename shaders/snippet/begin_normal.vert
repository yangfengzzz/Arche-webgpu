#ifndef OMIT_NORMAL
    #ifdef HAS_NORMAL
        vec3 normal = vec3( NORMAL );
    #endif

    #ifdef HAS_TANGENT
        vec4 tangent = vec4( TANGENT );
    #endif
#endif