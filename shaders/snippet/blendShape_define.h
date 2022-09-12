#ifdef HAS_BLENDSHAPE
    #ifdef HAS_BLENDSHAPE_TEXTURE
        layout(set = 0, binding = Blend_Shape_Texture_Location) uniform mediump texture2DArray u_blendShapeTexture;
        layout(set = 0, binding = Blend_Shape_Sampler_Location) uniform mediump sampler u_blendShapeSampler;
        layout(set = 0, binding = Blend_Shape_Texture_Info_Location) uniform ivec3 u_blendShapeTextureInfo;
        layout(set = 0, binding = Blend_Shape_Weights_Location) uniform u_blendShapeWeights {
            float blendShapeWeights[HAS_BLENDSHAPE_COUNT];
        };
    #else
        layout(location = Position_BS0) in vec3 POSITION_BS0;
        layout(location = Position_BS1) in vec3 POSITION_BS1;
        layout(location = Position_BS2) in vec3 POSITION_BS2;
        layout(location = Position_BS3) in vec3 POSITION_BS3;

            #if defined( HAS_BLENDSHAPE_NORMAL ) || defined( HAS_BLENDSHAPE_TANGENT )
                #ifdef HAS_BLENDSHAPE_NORMAL
                    layout(location = Normal_BS0) in vec3 NORMAL_BS0;
                    layout(location = Normal_BS1) in vec3 NORMAL_BS1;
                    layout(location = Normal_BS2) in vec3 NORMAL_BS2;
                    layout(location = Normal_BS3) in vec3 NORMAL_BS3;
                #endif

                #ifdef HAS_BLENDSHAPE_TANGENT
                    layout(location = Tangent_BS0) in vec3 TANGENT_BS0;
                    layout(location = Tangent_BS1) in vec3 TANGENT_BS1;
                    layout(location = Tangent_BS2) in vec3 TANGENT_BS2;
                    layout(location = Tangent_BS3) in vec3 TANGENT_BS3;
                #endif
                layout(set = 0, binding = Blend_Shape_Weights_Location) uniform u_blendShapeWeights {
                    float blendShapeWeights[4];
                };
            #else
                layout(location = Position_BS4) in vec3 POSITION_BS4;
                layout(location = Position_BS5) in vec3 POSITION_BS5;
                layout(location = Position_BS6) in vec3 POSITION_BS6;
                layout(location = Position_BS7) in vec3 POSITION_BS7;
                layout(set = 0, binding = Blend_Shape_Weights_Location) uniform u_blendShapeWeights {
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
