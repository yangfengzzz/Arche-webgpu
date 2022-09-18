float computeSpecularOcclusion(float ambientOcclusion, float roughness, float dotNV ) {
    return saturate( pow( dotNV + ambientOcclusion, exp2( - 16.0 * roughness - 1.0 ) ) - 1.0 + ambientOcclusion );
}

float getAARoughnessFactor(vec3 normal) {
    // Kaplanyan 2016, "Stable specular highlights"
    // Tokuyoshi 2017, "Error Reduction and Simplification for Shading Anti-Aliasing"
    // Tokuyoshi and Kaplanyan 2019, "Improved Geometric Specular Antialiasing"
    vec3 dxy = max( abs(dFdx(normal)), abs(dFdy(normal)) );
    return 0.04 + max( max(dxy.x, dxy.y), dxy.z );
}

void initGeometry(out Geometry geometry){
    geometry.position = v_pos;
    geometry.viewDir =  normalize(u_cameraPos - v_pos);

    #if defined(HAS_NORMAL_TEXTURE) || defined(HAS_CLEARCOATNORMAL_TEXTURE)
        mat3 tbn = getTBN();
    #endif

    #ifdef HAS_NORMAL_TEXTURE
        vec3 N = texture(sampler2D(u_normalTexture, u_normalSampler), v_uv).rgb;
        geometry.normal = normalize(tbn * ((2.0 * N - 1.0) * vec3(u_normalIntensity, u_normalIntensity, 1.0)));
    #else
        geometry.normal = getNormal();
    #endif

    geometry.dotNV = saturate( dot(geometry.normal, geometry.viewDir) );


    #ifdef HAS_CLEARCOAT
        #ifdef HAS_CLEARCOATNORMAL_TEXTURE
            vec3 N = texture(sampler2D(u_clearCoatNormalTexture, u_clearCoatNormalSampler), v_uv).rgb;
            geometry.clearCoatNormal = normalize(tbn * ((2.0 * N - 1.0) * vec3(u_normalIntensity, u_normalIntensity, 1.0)));
        #else
            geometry.clearCoatNormal = getNormal();
        #endif
        geometry.clearCoatDotNV = saturate( dot(geometry.clearCoatNormal, geometry.viewDir) );
    #endif

}

void initMaterial(out Material material, const in Geometry geometry){
    vec4 baseColor = u_baseColor;
    float metal = u_metal;
    float roughness = u_roughness;
#ifndef IS_METALLIC_WORKFLOW
    vec3 specularColor = u_PBRSpecularColor;
    float glossiness = u_glossiness;
#endif
    float alphaCutoff = alpha_cutoff;

    #ifdef HAS_BASE_TEXTURE
        vec4 baseTextureColor = texture(sampler2D(u_baseColorTexture, u_baseColorSampler), v_uv);
        #ifndef COLORSPACE_GAMMA
            baseTextureColor = gammaToLinear(baseTextureColor);
        #endif
        baseColor *= baseTextureColor;
    #endif

    #ifdef HAS_VERTEXCOLOR
        baseColor *= v_color;
    #endif


    #ifdef NEED_ALPHA_CUTOFF
        if( baseColor.a < alphaCutoff ) {
            discard;
        }
    #endif

    #ifdef HAS_ROUGHNESSMETALLIC_TEXTURE
        vec4 metalRoughMapColor = texture(sampler2D(u_roughnessMetallicTexture, u_roughnessMetallicSampler), v_uv );
        roughness *= metalRoughMapColor.g;
        metal *= metalRoughMapColor.b;
    #endif

    #ifdef HAS_SPECULARGLOSSINESS_TEXTURE
        vec4 specularGlossinessColor = texture(sampler2D(u_specularGlossinessTexture, u_specularGlossinessSampler), v_uv );
        #ifndef COLORSPACE_GAMMA
            specularGlossinessColor = gammaToLinear(specularGlossinessColor);
        #endif
        specularColor *= specularGlossinessColor.rgb;
        glossiness *= specularGlossinessColor.a;
    #endif


    #ifdef IS_METALLIC_WORKFLOW
        material.diffuseColor = baseColor.rgb * ( 1.0 - metal );
        material.specularColor = mix( vec3( 0.04), baseColor.rgb, metal );
        material.roughness = roughness;
    #else
        float specularStrength = max( max( specularColor.r, specularColor.g ), specularColor.b );
        material.diffuseColor = baseColor.rgb * ( 1.0 - specularStrength );
        material.specularColor = specularColor;
        material.roughness = 1.0 - glossiness;
    #endif

    material.roughness = max(material.roughness, getAARoughnessFactor(geometry.normal));

    #ifdef HAS_CLEARCOAT
        material.clearCoat = u_clearCoat;
        material.clearCoatRoughness = u_clearCoatRoughness;
        #ifdef HAS_CLEARCOAT_TEXTURE
            material.clearCoat *= texture(sampler2D(u_clearCoatTexture, u_clearCoatSampler), v_uv ).r;
        #endif
        #ifdef HAS_CLEARCOATROUGHNESS_TEXTURE
            material.clearCoatRoughness *= texture(sampler2D(u_clearCoatRoughnessTexture, u_clearCoatRoughnessSampler), v_uv ).g;
        #endif
        material.clearCoat = saturate( material.clearCoat );
        material.clearCoatRoughness = max(material.clearCoatRoughness, getAARoughnessFactor(geometry.clearCoatNormal));
    #endif

    material.opacity = baseColor.a;
}
