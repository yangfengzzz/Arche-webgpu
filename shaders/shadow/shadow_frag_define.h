#ifdef CASCADED_SHADOW_MAP
    #ifdef RECEIVE_SHADOWS
        #define CALCULATE_SHADOWS

        // intensity, resolution, sunIndex
        layout(set = 0, binding = 51) uniform u_shadowInfo {
            vec3 shadow_info;
        };

        layout(set = 0, binding = 52) uniform u_viewProjMatFromLight {
            mat4 view_proj_mat_from_light[4];
        };

        layout(set = 0, binding = 53) uniform u_shadowSplitSpheres {
            vec4 shadow_split_spheres[4];
        };

        layout(set = 0, binding = 54) uniform texture2D u_shadowTexture;
        layout(set = 0, binding = 55) uniform samplerShadow u_shadowSampler;

        #define SAMPLE_TEXTURE2D_SHADOW(coord3) textureLod(sampler2DShadow(u_shadowTexture, u_shadowSampler), coord3, 0.0)

        const vec2 offsets[4] = vec2[](
                vec2(0, 0),
                vec2(0.5, 0),
                vec2(0, 0.5),
                vec2(0.5, 0.5)
        );

        int computeCascadeIndex(vec3 positionWS) {
            vec3 fromCenter0 = positionWS - shadow_split_spheres[0].xyz;
            vec3 fromCenter1 = positionWS - shadow_split_spheres[1].xyz;
            vec3 fromCenter2 = positionWS - shadow_split_spheres[2].xyz;
            vec3 fromCenter3 = positionWS - shadow_split_spheres[3].xyz;

            vec4 comparison = vec4(
                dot(fromCenter0, fromCenter0) < shadow_split_spheres[0].w,
                dot(fromCenter1, fromCenter1) < shadow_split_spheres[1].w,
                dot(fromCenter2, fromCenter2) < shadow_split_spheres[2].w,
                dot(fromCenter3, fromCenter3) < shadow_split_spheres[3].w);
            comparison.yzw = clamp(comparison.yzw - comparison.xyz,0.0,1.0);//keep the nearest
            vec4 indexCoefficient = vec4(4.0,3.0,2.0,1.0);
            int index = 4 - int(dot(comparison, indexCoefficient));
            return index;
        }

        vec3 getShadowCoord() {
            int cascadeIndex = computeCascadeIndex(v_pos);
            vec2 offsets = offsets[cascadeIndex];
            mat4 viewProjMatFromLight = view_proj_mat_from_light[cascadeIndex];

        #if CASCADED_COUNT == 1
            float scaleX = 1.0;
            float scaleY = 1.0;
        #endif

        #if CASCADED_COUNT == 2
            float scaleX = 0.5;
            float scaleY = 1.0;
        #endif

        #if CASCADED_COUNT == 4
            float scaleX = 0.5;
            float scaleY = 0.5;
        #endif

            vec4 positionFromLight = viewProjMatFromLight * vec4(v_pos, 1.0);
            vec3 shadowCoord = positionFromLight.xyz / positionFromLight.w;
            shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
            vec3 coord = shadowCoord.xyz;
            coord.y = 1-coord.y;

            coord.x *= scaleX;
            coord.y *= scaleY;
            coord.xy += offsets;
            return coord;
        }

        #if SHADOW_MODE == 2
        float sampleShadowMapFiltered4(vec3 shadowCoord, float size) {
            vec2 shadowMapSize = vec2(0.5) / vec2(size, size);

            float attenuation;
            vec4 attenuation4;
            vec2 offset=shadowMapSize.xy/2.0;
            vec3 shadowCoord0=shadowCoord + vec3(-offset,0.0);
            vec3 shadowCoord1=shadowCoord + vec3(offset.x,-offset.y,0.0);
            vec3 shadowCoord2=shadowCoord + vec3(-offset.x,offset.y,0.0);
            vec3 shadowCoord3=shadowCoord + vec3(offset,0.0);
            attenuation4.x = SAMPLE_TEXTURE2D_SHADOW(shadowCoord0);
            attenuation4.y = SAMPLE_TEXTURE2D_SHADOW(shadowCoord1);
            attenuation4.z = SAMPLE_TEXTURE2D_SHADOW(shadowCoord2);
            attenuation4.w = SAMPLE_TEXTURE2D_SHADOW(shadowCoord3);
            attenuation = dot(attenuation4, vec4(0.25));
            return attenuation;
        }
        #endif

        #if SHADOW_MODE == 3
        #include <shadow_sample_tent>

        float sampleShadowMapFiltered9(vec3 shadowCoord, float size) {
            float attenuation;
            float fetchesWeights[9];
            vec2 fetchesUV[9];
            sampleShadowComputeSamplesTent5x5(size, shadowCoord.xy, fetchesWeights, fetchesUV);
            attenuation = fetchesWeights[0] * SAMPLE_TEXTURE2D_SHADOW(vec3(fetchesUV[0].xy, shadowCoord.z));
            attenuation += fetchesWeights[1] * SAMPLE_TEXTURE2D_SHADOW(vec3(fetchesUV[1].xy, shadowCoord.z));
            attenuation += fetchesWeights[2] * SAMPLE_TEXTURE2D_SHADOW(vec3(fetchesUV[2].xy, shadowCoord.z));
            attenuation += fetchesWeights[3] * SAMPLE_TEXTURE2D_SHADOW(vec3(fetchesUV[3].xy, shadowCoord.z));
            attenuation += fetchesWeights[4] * SAMPLE_TEXTURE2D_SHADOW(vec3(fetchesUV[4].xy, shadowCoord.z));
            attenuation += fetchesWeights[5] * SAMPLE_TEXTURE2D_SHADOW(vec3(fetchesUV[5].xy, shadowCoord.z));
            attenuation += fetchesWeights[6] * SAMPLE_TEXTURE2D_SHADOW(vec3(fetchesUV[6].xy, shadowCoord.z));
            attenuation += fetchesWeights[7] * SAMPLE_TEXTURE2D_SHADOW(vec3(fetchesUV[7].xy, shadowCoord.z));
            attenuation += fetchesWeights[8] * SAMPLE_TEXTURE2D_SHADOW(vec3(fetchesUV[8].xy, shadowCoord.z));
            return attenuation;
        }
        #endif

        float sampleShadowMap() {
            vec3 shadowCoord = getShadowCoord();
            float attenuation = 1.0;
            if(shadowCoord.z > 0.0 && shadowCoord.z < 1.0) {
        #if SHADOW_MODE == 1
                attenuation = SAMPLE_TEXTURE2D_SHADOW(shadowCoord);
        #endif

        #if SHADOW_MODE == 2
                attenuation = sampleShadowMapFiltered4(shadowCoord, shadow_info.y);
        #endif

        #if SHADOW_MODE == 3
                attenuation = sampleShadowMapFiltered9(shadowCoord, shadow_info.y);
        #endif
                attenuation = mix(1.0, attenuation, shadow_info.x);
            }
            return attenuation;
        }
    #endif
#endif