#ifdef HAS_UV
    v_uv = TEXCOORD_0;
#else
    // may need this calculate normal
    v_uv = vec2( 0., 0. );
#endif

#ifdef HAS_UV1
    v_uv1 = TEXCOORD_1;
#endif

#ifdef NEED_TILINGOFFSET
    v_uv = v_uv * tilingOffset.xy + tilingOffset.zw;
#endif