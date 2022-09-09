#ifndef OMIT_NORMAL
    #ifdef O3_HAS_NORMAL
        v_normal = normalize( mat3(u_normalMat) * normal );

        #if defined(O3_HAS_TANGENT) && ( defined(NORMALTEXTURE) || defined(HAS_CLEARCOATNORMALTEXTURE) )
            vec3 normalW = normalize( mat3(u_normalMat) * normal.xyz );
            vec3 tangentW = normalize( mat3(u_normalMat) * tangent.xyz );
            vec3 bitangentW = cross( normalW, tangentW ) * tangent.w;

            v_TBN = mat3( tangentW, bitangentW, normalW );
        #endif
    #endif
#endif