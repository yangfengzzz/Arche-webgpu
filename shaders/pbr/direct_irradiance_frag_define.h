void addDirectRadiance(vec3 incidentDirection, vec3 color, Geometry geometry, Material material, inout ReflectedLight reflectedLight) {
    float attenuation = 1.0;

#ifdef HAS_CLEARCOAT
    float clearCoatDotNL = saturate( dot( geometry.clearCoatNormal, incidentDirection ) );
    vec3 clearCoatIrradiance = clearCoatDotNL * color;

    reflectedLight.directSpecular += material.clearCoat * clearCoatIrradiance * BRDF_Specular_GGX( incidentDirection, geometry.viewDir, geometry.clearCoatNormal, vec3( 0.04 ), material.clearCoatRoughness );
    attenuation -= material.clearCoat * F_Schlick(geometry.clearCoatDotNV);
#endif

    float dotNL = saturate( dot( geometry.normal, incidentDirection ) );
    vec3 irradiance = dotNL * color * PI;

    reflectedLight.directSpecular += attenuation * irradiance * BRDF_Specular_GGX( incidentDirection, geometry.viewDir, geometry.normal, material.specularColor, material.roughness);
    reflectedLight.directDiffuse += attenuation * irradiance * BRDF_Diffuse_Lambert( material.diffuseColor );
}

#ifdef DIRECT_LIGHT_COUNT
void addDirectionalDirectLightRadiance(DirectLight directionalLight, Geometry geometry, Material material, inout ReflectedLight reflectedLight) {
    vec3 color = directionalLight.color;
    vec3 direction = -directionalLight.direction;
    addDirectRadiance( direction, color, geometry, material, reflectedLight );
}
#endif

#ifdef POINT_LIGHT_COUNT
void addPointDirectLightRadiance(PointLight pointLight, Geometry geometry, Material material, inout ReflectedLight reflectedLight) {
    vec3 lVector = pointLight.position - geometry.position;
    vec3 direction = normalize( lVector );

    float lightDistance = length( lVector );

    vec3 color = pointLight.color;
    color *= clamp(1.0 - pow(lightDistance/pointLight.distance, 4.0), 0.0, 1.0);

    addDirectRadiance( direction, color, geometry, material, reflectedLight );
}
#endif

#ifdef SPOT_LIGHT_COUNT
void addSpotDirectLightRadiance(SpotLight spotLight, Geometry geometry, Material material, inout ReflectedLight reflectedLight) {
    vec3 lVector = spotLight.position - geometry.position;
    vec3 direction = normalize( lVector );

    float lightDistance = length( lVector );
    float angleCos = dot( direction, -spotLight.direction );

    float spotEffect = smoothstep( spotLight.penumbraCos, spotLight.angleCos, angleCos );
    float decayEffect = clamp(1.0 - pow(lightDistance/spotLight.distance, 4.0), 0.0, 1.0);

    vec3 color = spotLight.color;
    color *= spotEffect * decayEffect;

    addDirectRadiance( direction, color, geometry, material, reflectedLight );
}
#endif

void addTotalDirectRadiance(Geometry geometry, Material material, inout ReflectedLight reflectedLight){
    float shadowAttenuation = 1.0;

#ifdef DIRECT_LIGHT_COUNT
    shadowAttenuation = 1.0;
    #ifdef CALCULATE_SHADOWS
        #ifdef CASCADED_SHADOW_MAP
            shadowAttenuation *= sampleShadowMap();
            int sunIndex = int(shadow_info.z);
        #endif
    #endif

    DirectLight directionalLight;
    for ( int i = 0; i < DIRECT_LIGHT_COUNT; i ++ ) {
        directionalLight.color = direct_light[i].color;
#ifdef CALCULATE_SHADOWS
    #ifdef CASCADED_SHADOW_MAP
        if (i == sunIndex) {
            directionalLight.color *= shadowAttenuation;
        }
    #endif
#endif
        directionalLight.direction = direct_light[i].direction;
        addDirectionalDirectLightRadiance( directionalLight, geometry, material, reflectedLight );
    }
#endif

#ifdef POINT_LIGHT_COUNT
    PointLight pointLight;

    for ( int i = 0; i < POINT_LIGHT_COUNT; i ++ ) {
        pointLight = point_light[i];
        addPointDirectLightRadiance( pointLight, geometry, material, reflectedLight );
    }
#endif

#ifdef SPOT_LIGHT_COUNT
    SpotLight spotLight;

    for ( int i = 0; i < SPOT_LIGHT_COUNT; i ++ ) {
        spotLight = spot_light[i];
        addSpotDirectLightRadiance( spotLight, geometry, material, reflectedLight );
    }
#endif
}
