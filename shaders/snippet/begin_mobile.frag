    vec4 ambient = vec4(0.0);
    vec4 emission = u_emissiveColor;
    vec4 diffuse = u_baseColor;
    vec4 specular = u_specularColor;

    

    #ifdef EMISSIVETEXTURE
        vec4 emissiveTextureColor = texture(u_emissiveTexture, v_uv);
        #ifndef COLORSPACE_GAMMA
            emissiveTextureColor = gammaToLinear(emissiveTextureColor);
        #endif
        emission *= emissiveTextureColor;

    #endif

    #ifdef BASETEXTURE
        vec4 diffuseTextureColor = texture(u_baseTexture, v_uv);
        #ifndef COLORSPACE_GAMMA
            diffuseTextureColor = gammaToLinear(diffuseTextureColor);
        #endif
        diffuse *= diffuseTextureColor;

    #endif

     #ifdef HAS_VERTEXCOLOR

        diffuse *= v_color;

    #endif

    #ifdef SPECULAR_TEXTURE
        vec4 specularTextureColor = texture(u_specularTexture, v_uv);
        #ifndef COLORSPACE_GAMMA
            specularTextureColor = gammaToLinear(specularTextureColor);
        #endif
        specular *= specularTextureColor;

    #endif

    ambient = vec4(u_envMapLight.diffuse * u_envMapLight.diffuseIntensity, 1.0) * diffuse;