vec3 getNormal(){
    #ifdef HAS_NORMAL
        vec3 normal = normalize(v_normal);
    #else
        vec3 pos_dx = dFdx(v_pos);
        vec3 pos_dy = dFdy(v_pos);
        vec3 normal = normalize( cross(pos_dx, pos_dy) );
    #endif

    // normal *= float( gl_FrontFacing ) * 2.0 - 1.0;
    return normal;
}

mat3 getTBN(){
    #if defined(HAS_NORMAL) && defined(HAS_TANGENT) && ( defined(HAS_NORMAL_TEXTURE) || defined(HAS_CLEARCOATNORMAL_TEXTURE) )
        mat3 tbn = v_TBN;
    #else
        vec3 normal = getNormal();
        vec3 position = v_pos;
        vec2 uv = gl_FrontFacing? v_uv: -v_uv;

        // ref: http://www.thetenthplanet.de/archives/1180
        // get edge vectors of the pixel triangle
        vec3 dp1 = dFdx(position);
        vec3 dp2 = dFdy(position);
        vec2 duv1 = dFdx(uv);
        vec2 duv2 = dFdy(uv);

        // solve the linear system
        vec3 dp2perp = cross(dp2, normal);
        vec3 dp1perp = cross(normal, dp1);
        vec3 tangent = dp2perp * duv1.x + dp1perp * duv2.x;
        vec3 binormal = dp2perp * duv1.y + dp1perp * duv2.y;

        // construct a scale-invariant frame
        float invmax = inversesqrt(max(dot(tangent, tangent), dot(binormal, binormal)));
        mat3 tbn = mat3(tangent * invmax, binormal * invmax, normal);
    #endif
	
    return tbn;
}
