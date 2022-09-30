#ifdef HAS_SKIN
    #ifdef USE_JOINT_TEXTURE
        mat4 skinMatrix =
            WEIGHTS_0.x * getJointMatrix(sampler2d(u_jointTexture, u_jointSampler), JOINTS_0.x ) +
            WEIGHTS_0.y * getJointMatrix(sampler2d(u_jointTexture, u_jointSampler), JOINTS_0.y ) +
            WEIGHTS_0.z * getJointMatrix(sampler2d(u_jointTexture, u_jointSampler), JOINTS_0.z ) +
            WEIGHTS_0.w * getJointMatrix(sampler2d(u_jointTexture, u_jointSampler), JOINTS_0.w );

    #else
        mat4 skinMatrix =
            WEIGHTS_0.x * joint_matrix[ JOINTS_0.x ] +
            WEIGHTS_0.y * joint_matrix[ JOINTS_0.y ] +
            WEIGHTS_0.z * joint_matrix[ JOINTS_0.z ] +
            WEIGHTS_0.w * joint_matrix[ JOINTS_0.w ];
    #endif

    position = skinMatrix * position;

    #if defined(HAS_NORMAL) && !defined(OMIT_NORMAL)
        normal = vec4( skinMatrix * vec4( normal, 0.0 ) ).xyz;
        #if defined(HAS_TANGENT) && ( defined(HAS_NORMAL_TEXTURE) || defined(HAS_CLEARCOATNORMAL_TEXTURE) )
            tangent.xyz = vec4( skinMatrix * vec4( tangent.xyz, 0.0 ) ).xyz;
        #endif

    #endif
#endif
