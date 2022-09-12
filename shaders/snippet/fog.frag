#ifdef HAS_FOG
    float fogDepth = length( v_fogDepth );
    #ifdef FOG_EXP2
        float fogFactor = whiteCompliment( exp2( - u_fogDensity * u_fogDensity * fogDepth * fogDepth * LOG2 ) );
    #else
        float fogFactor = smoothstep( u_fogNear, u_fogFar, fogDepth );
    #endif
        o_color.rgb = mix( o_color.rgb, u_fogColor, fogFactor );
#endif
