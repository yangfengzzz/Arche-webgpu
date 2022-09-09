#ifdef HAS_BLENDSHAPE
    #ifdef HAS_BLENDSHAPE_TEXTURE
        layout(location = 10) uniform mediump sampler2DArray u_blendShapeTexture;
        layout(location = 11) uniform ivec3 u_blendShapeTextureInfo;
        layout(set = 0, binding = 8) uniform u_blendShapeWeights {
            float blendShapeWeights[HAS_BLENDSHAPE_COUNT];
        };
    #else
        layout(location = 10) in vec3 POSITION_BS0;
        layout(location = 11) in vec3 POSITION_BS1;
        layout(location = 12) in vec3 POSITION_BS2;
        layout(location = 13) in vec3 POSITION_BS3;

            #if defined( HAS_BLENDSHAPE_NORMAL ) || defined( HAS_BLENDSHAPE_TANGENT )
                #ifdef HAS_BLENDSHAPE_NORMAL
                    layout(location = 16) in vec3 NORMAL_BS0;
                    layout(location = 17) in vec3 NORMAL_BS1;
                    layout(location = 18) in vec3 NORMAL_BS2;
                    layout(location = 19) in vec3 NORMAL_BS3;
                #endif

                #ifdef HAS_BLENDSHAPE_TANGENT
                    layout(location = 20) in vec3 TANGENT_BS0;
                    layout(location = 21) in vec3 TANGENT_BS1;
                    layout(location = 22) in vec3 TANGENT_BS2;
                    layout(location = 23) in vec3 TANGENT_BS3;
                #endif
                layout(set = 0, binding = 8) uniform u_blendShapeWeights {
                    float blendShapeWeights[4];
                };
            #else
                ayout(location = 16) in vec3 POSITION_BS4;
                ayout(location = 17) in vec3 POSITION_BS5;
                ayout(location = 18) in vec3 POSITION_BS6;
                ayout(location = 19) in vec3 POSITION_BS7;
                layout(set = 0, binding = 8) uniform u_blendShapeWeights {
                    float blendShapeWeights[4];
                };
            #endif
    #endif

    #ifdef HAS_BLENDSHAPE_TEXTURE
        vec3 getBlendShapeVertexElement(int blendShapeIndex, int vertexElementIndex) {
                int y = vertexElementIndex / u_blendShapeTextureInfo.y;
                int x = vertexElementIndex - y * u_blendShapeTextureInfo.y;
                ivec3 uv = ivec3(x, y , blendShapeIndex);
                return texelFetch(u_blendShapeTexture, uv, 0).xyz;
        }
    #endif
#endif
