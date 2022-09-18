    vec4 ambient = vec4(0.0);
    vec4 emission = u_emissiveColor;
    vec4 diffuse = u_baseColor;
    vec4 specular = u_specularColor;

    #ifdef HAS_EMISSIVE_TEXTURE
        vec4 emissiveTextureColor = texture(sampler2D(u_emissiveTexture, u_emissiveSampler), v_uv);
        #ifndef COLORSPACE_GAMMA
            emissiveTextureColor = gammaToLinear(emissiveTextureColor);
        #endif
        emission *= emissiveTextureColor;
    #endif

    #ifdef HAS_BASE_TEXTURE
        vec4 diffuseTextureColor = texture(sampler2D(u_baseColorTexture, u_baseColorSampler), v_uv);
        #ifndef COLORSPACE_GAMMA
            diffuseTextureColor = gammaToLinear(diffuseTextureColor);
        #endif
        diffuse *= diffuseTextureColor;
    #endif

     #ifdef HAS_VERTEXCOLOR
        diffuse *= v_color;
    #endif

    #ifdef HAS_SPECULAR_TEXTURE
        vec4 specularTextureColor = texture(sampler2D(u_specularTexture, u_specularSampler), v_uv);
        #ifndef COLORSPACE_GAMMA
            specularTextureColor = gammaToLinear(specularTextureColor);
        #endif
        specular *= specularTextureColor;
    #endif

    ambient = vec4(env_map_light.diffuse * env_map_light.diffuse_intensity, 1.0) * diffuse;
