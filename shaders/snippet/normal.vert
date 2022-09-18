#ifndef OMIT_NORMAL
    #ifdef HAS_NORMAL
        v_normal = normalize( mat3(u_normalMat) * normal );

        #if defined(HAS_TANGENT) && ( defined(HAS_NORMAL_TEXTURE) || defined(HAS_CLEARCOATNORMAL_TEXTURE) )
            vec3 normalW = normalize( mat3(u_normalMat) * normal.xyz );
            vec3 tangentW = normalize( mat3(u_normalMat) * tangent.xyz );
            vec3 bitangentW = cross( normalW, tangentW ) * tangent.w;

            v_TBN = mat3( tangentW, bitangentW, normalW );
        #endif
    #endif
#endif
