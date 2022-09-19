    #ifdef HAS_NORMAL_TEXTURE
        mat3 tbn = getTBN();
        vec3 N = texture(sampler2D(u_normalTexture, u_normalSampler), v_uv).rgb;
        N = normalize(tbn * ((2.0 * N - 1.0) * vec3(u_normalIntensity, u_normalIntensity, 1.0)));
    #else
        vec3 N = getNormal();
    #endif

    vec3 lightDiffuse = vec3( 0.0, 0.0, 0.0 );
    vec3 lightSpecular = vec3( 0.0, 0.0, 0.0 );
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
        for( int i = 0; i < DIRECT_LIGHT_COUNT; i++ ) {
            directionalLight.color = direct_light[i].color * shadowAttenuation;
#ifdef CALCULATE_SHADOWS
    #ifdef CASCADED_SHADOW_MAP
                if (i == sunIndex) {
                    directionalLight.color *= shadowAttenuation;
                }
    #endif
#endif
            directionalLight.direction = direct_light[i].direction;

            float d = max(dot(N, -directionalLight.direction), 0.0);
            lightDiffuse += directionalLight.color * d;

            vec3 halfDir = normalize( V - directionalLight.direction );
            float s = pow( clamp( dot( N, halfDir ), 0.0, 1.0 ), u_shininess );
            lightSpecular += directionalLight.color * s;
        }
    #endif

    #ifdef POINT_LIGHT_COUNT
        PointLight pointLight;
        for( int i = 0; i < POINT_LIGHT_COUNT; i++ ) {
            pointLight.color = point_light[i].color;
            pointLight.position = point_light[i].position;
            pointLight.distance = point_light[i].distance;

            vec3 direction = v_pos - pointLight.position;
            float dist = length( direction );
            direction /= dist;
            float decay = clamp(1.0 - pow(dist / pointLight.distance, 4.0), 0.0, 1.0);

            float d =  max( dot( N, -direction ), 0.0 ) * decay;
            lightDiffuse += pointLight.color * d;

            vec3 halfDir = normalize( V - direction );
            float s = pow( clamp( dot( N, halfDir ), 0.0, 1.0 ), u_shininess )  * decay;
            lightSpecular += pointLight.color * s;
        }
    #endif

    #ifdef SPOT_LIGHT_COUNT
        SpotLight spotLight;
        for( int i = 0; i < SPOT_LIGHT_COUNT; i++) {
            spotLight.color = spot_light[i].color;
            spotLight.position = spot_light[i].position;
            spotLight.direction = spot_light[i].direction;
            spotLight.distance = spot_light[i].distance;
            spotLight.angleCos = spot_light[i].angleCos;
            spotLight.penumbraCos = spot_light[i].penumbraCos;

            vec3 direction = spotLight.position - v_pos;
            float lightDistance = length( direction );
            direction /= lightDistance;
            float angleCos = dot( direction, -spotLight.direction );
            float decay = clamp(1.0 - pow(lightDistance/spotLight.distance, 4.0), 0.0, 1.0);
            float spotEffect = smoothstep( spotLight.penumbraCos, spotLight.angleCos, angleCos );
            float decayTotal = decay * spotEffect;
            float d = max( dot( N, direction ), 0.0 )  * decayTotal;
            lightDiffuse += spotLight.color * d;

            vec3 halfDir = normalize( V + direction );
            float s = pow( clamp( dot( N, halfDir ), 0.0, 1.0 ), u_shininess ) * decayTotal;
            lightSpecular += spotLight.color * s;
        }
    #endif

    diffuse *= vec4( lightDiffuse, 1.0 );
    specular *= vec4( lightSpecular, 1.0 );

    #ifdef NEED_ALPHA_CUTOFF
        if( diffuse.a < alpha_cutoff ) {
            discard;
        }
    #endif
